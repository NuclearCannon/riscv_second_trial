#include "MMIO.hpp"
#include "../debug_utils.hpp"

// MMIO写入函数（32位写入）
void write_mmio_32(uintptr_t reg_offset, uint32_t value) {
    *(volatile uint32_t*)(virtio_blk_base + reg_offset) = value;
    // 内存屏障确保立即写入
    memory_barrier();
    if(*(volatile uint32_t*)(virtio_blk_base + reg_offset)!=value)
    {
        kprint::printf("Error on write %p\n",reg_offset);
        panic("Error on write_mmio_32\n");
    }
}
// MMIO读出函数（32位读出）
uint32_t read_mmio_32(uintptr_t reg_offset) {
    memory_barrier();
    return *(volatile uint32_t*)(virtio_blk_base + reg_offset);
}
// MMIO写入函数（64位写入）
void write_mmio_64(uintptr_t reg_offset, uint64_t value) {
    write_mmio_32(reg_offset, (uint32)(value & 0xFFFFFFFF));
    write_mmio_32(reg_offset+4, (uint32)(value >> 32));
}
// MMIO读出函数（64位读出）
uint64_t read_mmio_64(uintptr_t reg_offset) {
    uint64 temp = 0;
    temp |= read_mmio_32(reg_offset);
    uint64 temp2 = read_mmio_32(reg_offset+4);
    temp |= (temp2<<32);
    return temp;
}

// 设置设备状态寄存器
void virtio_set_status(uint8_t status) {
    write_mmio_32(VIRTIO_MMIO_STATUS, status);
}

// 读取设备状态寄存器
uint8_t virtio_get_status() {
    // memory_barrier();
    // volatile uint8_t *reg = (volatile uint8_t *)(virtio_blk_base + VIRTIO_MMIO_STATUS);
    // return *reg;

    return read_mmio_32(VIRTIO_MMIO_STATUS) & 0xFF;
}


// VirtIO 描述符结构（设备可见内存）
struct virtq_desc {
    uint64_t addr;  // 物理地址
    uint32_t len;
    uint16_t flags; // VRING_DESC_F_NEXT 等
    uint16_t next;
} __attribute__((packed));

struct virtq_avail {
    uint16_t flags;     // 环形缓冲区的标志位
    uint16_t idx;       // 下一个可用位置的索引
    uint16_t ring[100];    // 存储描述符链的头部索引
};
// 100是随便打的……
struct virtq_used_elem {
    uint32_t id;    // 完成的描述符链头部索引
    uint32_t len;   // 实际写入的数据长度
};
struct virtq_used {
    uint16_t flags;
    uint16_t idx;
    struct virtq_used_elem ring[100]; // 包含设备写入的完成信息
};



// 硬编码分配内存（需确保物理地址固定！）


#define VIRTIO_QUEUE_SIZE 8 // 描述符环大小
struct virtq_desc vq_desc[VIRTIO_QUEUE_SIZE] __attribute__((aligned(4096)));
struct virtq_avail vq_avail __attribute__((aligned(4096)));
struct virtq_used vq_used __attribute__((aligned(4096)));

// 全局 VirtIO 队列状态
struct {
    struct virtq_desc* desc;    // 描述符数组
    struct virtq_avail* avail;  // 可用环
    struct virtq_used* used;    // 已用环
    uint16_t queue_size;        // 队列长度（必须是2的幂）
    uint16_t free_desc_head;    // 空闲描述符链表头
    //spinlock_t lock;            // 并发保护锁
} vq = {
    .desc = vq_desc,
    .avail = &vq_avail,
    .used = &vq_used,
    .queue_size = 8,
    .free_desc_head = 0
};

char disk_free[8];

struct {
    void *information;
    char status;
} disk_info[8];

// find a free descriptor, mark it non-free, return its index.
static int alloc_desc() {
    for (int i = 0; i < 8; i++) {
        if (disk_free[i]) {
            disk_free[i] = 0;
            return i;
        }
    }
    return -1;
}

// mark a descriptor as free.
static void free_desc(int i) {
    if (i >= 8) panic("virtio_disk_intr 1");
    if (disk_free[i]) panic("virtio_disk_intr 2");
    vq.desc[i].addr = 0;
    disk_free[i] = 1;
    // wakeup(&disk.free[0]);
}

// free a chain of descriptors.
static void free_chain(int i) {
    while (1) {
        free_desc(i);
        if (vq.desc[i].flags & VRING_DESC_F_NEXT)
            i = vq.desc[i].next;
        else
            break;
    }
}

static int alloc3_desc(int *idx) {
    for (int i = 0; i < 3; i++) {
        idx[i] = alloc_desc();
        if (idx[i] < 0) {
            for (int j = 0; j < i; j++) free_desc(idx[j]);
            return -1;
        }
    }
    return 0;
}



void virtio_blk_init() {
    kprint::printf("Enter virtio init!\n");
    // 初始化空闲描述符链表
    for (int i = 0; i < vq.queue_size; i++) {
        vq.desc[i].next = (i == vq.queue_size-1) ? 0xFFFF : i+1;
    }

    // 检查设备
    uint32_t magic = read_mmio_32(VIRTIO_MMIO_MAGIC_VALUE);
    if (magic != 0x74726976) panic("Bad VirtIO magic");
    uint32 version = read_mmio_32(VIRTIO_MMIO_VERSION);
    if (version != 1)panic("Bad version!");
    uint32 dev_id = read_mmio_32(VIRTIO_MMIO_DEVICE_ID);
    if (dev_id != 2)panic("Bad dev_id!");
    uint32 vendor_id = read_mmio_32(VIRTIO_MMIO_VENDOR_ID);
    if (vendor_id != 0x554d4551)panic("Bad vendor_id!");

    // 重置设备
    virtio_set_status(0);
    // 设置 ACKNOWLEDGE 状态
    // virtio_set_status(VIRTIO_STATUS_ACKNOWLEDGE);

    // 设置driver状态
    virtio_set_status(VIRTIO_STATUS_ACKNOWLEDGE | VIRTIO_STATUS_DRIVER);

    // 设置特性
    uint32 features = read_mmio_32(VIRTIO_MMIO_DEVICE_FEATURES);
    kprint::printf("设备特性：%p\n", features);
    features &= ~(1 << VIRTIO_BLK_F_RO);
    features &= ~(1 << VIRTIO_BLK_F_SCSI);
    features &= ~(1 << VIRTIO_BLK_F_CONFIG_WCE);
    features &= ~(1 << VIRTIO_BLK_F_MQ);
    features &= ~(1 << VIRTIO_F_ANY_LAYOUT);
    features &= ~(1 << VIRTIO_RING_F_EVENT_IDX);
    features &= ~(1 << VIRTIO_RING_F_INDIRECT_DESC);
    // features |= VIRTIO_F_VERSION_1;

    kprint::printf("欲将驱动特性改为：%p\n", features);

    // 设置FEATURES_OK状态（告诉设备：驱动特性准备好了）
    write_mmio_32(VIRTIO_MMIO_DRIVER_FEATURES, features);   // 写操作内部已经加入了内存屏障

    

    
   

    write_mmio_32(VIRTIO_MMIO_GUEST_PAGE_SIZE,4096);

    // 步骤4：设置队列0
    write_mmio_32(VIRTIO_MMIO_QUEUE_SEL, 0); // 选择队列0
    uint32_t max_queue_size = read_mmio_32(VIRTIO_MMIO_QUEUE_NUM_MAX);
    if (max_queue_size < 8) panic("Queue too small");
    write_mmio_32(VIRTIO_MMIO_QUEUE_NUM, 8); // 设置队列长度为8
    
    // 步骤5：分配描述符环内存（物理连续）
    uint64_t desc_phys = (uint64_t)(&vq_desc);
    
    // 步骤6：设置队列物理地址（Legacy模式使用QUEUE_PFN）
    write_mmio_64(VIRTIO_MMIO_QUEUE_PFN, desc_phys / 4096);
    
    // 步骤7：激活队列
    write_mmio_32(VIRTIO_MMIO_QUEUE_READY, 1);
    
    // 检查队列是否就绪
    uint32_t queue_ready = read_mmio_32(VIRTIO_MMIO_QUEUE_READY);
    kprint::printf("队列就绪状态: %p (应为 0x1)\n", queue_ready);
    if (queue_ready != 1) panic("队列未就绪1");
    // 步骤8：完成设备初始化
    virtio_set_status(VIRTIO_STATUS_ACKNOWLEDGE | VIRTIO_STATUS_DRIVER | VIRTIO_STATUS_FEATURES_OK | VIRTIO_STATUS_DRIVER_OK);

    // 检查队列是否就绪
    queue_ready = read_mmio_32(VIRTIO_MMIO_QUEUE_READY);
    kprint::printf("队列就绪状态: %p (应为 0x1)\n", queue_ready);
    if (queue_ready != 1) panic("队列未就绪2");

    kprint::printf("Leave virtio init!\n");
}

// 分配一个描述符链（返回头描述符索引）
static uint16_t alloc_desc_chain(int num) {

    
    // spin_lock(&vq.lock);
    
    uint16_t head = vq.free_desc_head;
    uint16_t prev = head;
    for (int i = 0; i < num; i++) {
        if (vq.free_desc_head == 0xFFFF) {
            // spin_unlock(&vq.lock);
            return 0xFFFF; // 无可用描述符
        }
        prev = vq.free_desc_head;
        vq.free_desc_head = vq.desc[vq.free_desc_head].next;
    }
    
    // spin_unlock(&vq.lock);
    return head;
}

// 释放描述符链
static void free_desc_chain(uint16_t head) {
    // spin_lock(&vq.lock);
    
    uint16_t last = head;
    while (vq.desc[last].flags & VRING_DESC_F_NEXT) {
        last = vq.desc[last].next;
    }
    vq.desc[last].next = vq.free_desc_head;
    vq.free_desc_head = head;
    
    // spin_unlock(&vq.lock);
}


#define VIRTIO_BLK_T_IN 0   // read the disk
#define VIRTIO_BLK_T_OUT 1  // write the disk


// 提交请求到设备队列
void virtio_submit_request(struct disk_request* req) {
    kprint::printf("Enter virtio_submit_request!\n");
    // 1. 分配两个描述符（请求头 + 数据缓冲区）
    uint16_t head = alloc_desc_chain(3);
    if (head == 0xFFFF) panic("No free descriptors");
    
    // 2. 构建请求头（符合VirtIO-blk规范）
    struct virtio_blk_req {
        uint32_t type;    // 0 = 读，1 = 写
        uint32_t reserved;
        uint64_t sector;
    } hdr = {
        .type = 0,        // VIRTIO_BLK_T_IN
        .sector = req->blkno * (4096 / 512)
    };

    if(req->do_write)
    {
        hdr.type = VIRTIO_BLK_T_OUT;
    }
    else
    {
        hdr.type = VIRTIO_BLK_T_IN;
    }
    hdr.reserved = 0;
    hdr.sector = req->blkno * (4096 / 512);
    
    // 3. 配置描述符链
    vq.desc[head].addr = (uint64)&hdr;
    vq.desc[head].len = sizeof(hdr);
    vq.desc[head].flags = VRING_DESC_F_NEXT;  // 链继续
    vq.desc[head].next = head + 1;
    
    vq.desc[head+1].addr = (uint64)req->buffer;
    vq.desc[head+1].len = 4096;
    if(req->do_write)
    {
        vq.desc[head+1].flags = 0;
    }
    else
    {
        vq.desc[head+1].flags = VRING_DESC_F_WRITE; // 设备写入
        // 好奇怪？
    }

    disk_info[head].status = 0;
    vq.desc[head+2].addr = (uint64)&disk_info[head].status;
    vq.desc[head+2].len = 1;
    vq.desc[head+2].flags = VRING_DESC_F_WRITE; 
    vq.desc[head+2].next = 0;
    disk_info[head].information = req->userdata;


    // 4. 更新可用环
    uint16_t avail_idx = vq.avail->idx % vq.queue_size;
    vq.avail->ring[avail_idx] = head;
    
    // 内存屏障确保设备看到顺序正确的写入
    memory_barrier();
    
    vq.avail->idx++;
    
    // 5. 通知设备
    write_mmio_32(VIRTIO_MMIO_QUEUE_NOTIFY, 0);
    
    // if(1)
    // {
    //     // 等待！
    //     while()
    // }
    
    // 6. 记录请求上下文（用于回调）
    // req->desc_head = head;
    // req->hdr = hdr; // 必须保持有效直到完成
    kprint::printf("Leave virtio_submit_request!\n");

}

