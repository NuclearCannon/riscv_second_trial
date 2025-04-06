#pragma once

#include "../types.hpp"
#include "../debug_utils.hpp"
#include "../kernel_print.hpp"



struct disk_request {
    char do_write;              // 如果为1，表示这是一个写请求，否则是一个读请求
    uint64 blkno;               // 你想读哪个块？
    void* buffer;               // 读操作的目的地缓冲区？
    void* userdata;             // callback的实参
    void (*callback)(void*);    // 一个参数为void*, 返回值为void的函数指针。
    // 当一个请求被满足时，会触发中断，中断响应函数会调用callback(userdata);

};

// stt
#define VIRTIO_MMIO_MAGIC_VALUE 0x000  // 0x74726976
#define VIRTIO_MMIO_VERSION 0x004      // version; 1 is legacy
#define VIRTIO_MMIO_DEVICE_ID 0x008    // device type; 1 is net, 2 is disk
#define VIRTIO_MMIO_VENDOR_ID 0x00c    // 0x554d4551
#define VIRTIO_MMIO_DEVICE_FEATURES 0x010
#define VIRTIO_MMIO_DRIVER_FEATURES 0x020
#define VIRTIO_MMIO_GUEST_PAGE_SIZE 0x028  // page size for PFN, write-only
#define VIRTIO_MMIO_QUEUE_SEL 0x030        // select queue, write-only
#define VIRTIO_MMIO_QUEUE_NUM_MAX 0x034  // max size of current queue, read-only
#define VIRTIO_MMIO_QUEUE_NUM 0x038      // size of current queue, write-only
#define VIRTIO_MMIO_QUEUE_ALIGN 0x03c    // used ring alignment, write-only
#define VIRTIO_MMIO_QUEUE_PFN 0x040  // physical page number for queue, read/write
#define VIRTIO_MMIO_QUEUE_READY 0x044       // ready bit
#define VIRTIO_MMIO_QUEUE_NOTIFY 0x050      // write-only
#define VIRTIO_MMIO_INTERRUPT_STATUS 0x060  // read-only
#define VIRTIO_MMIO_INTERRUPT_ACK 0x064     // write-only
#define VIRTIO_MMIO_STATUS 0x070            // read/write


// 设备状态寄存器标志（STATUS）
#define VIRTIO_STATUS_ACKNOWLEDGE      (1 << 0) // 设备已识别
#define VIRTIO_STATUS_DRIVER           (1 << 1) // 驱动已加载
#define VIRTIO_STATUS_DRIVER_OK        (1 << 2) // 驱动就绪
#define VIRTIO_STATUS_FEATURES_OK      (1 << 3) // 特性协商完成
#define VIRTIO_STATUS_DEVICE_NEEDS_RESET (1 << 6) // 设备需要复位
#define VIRTIO_STATUS_FAILED           (1 << 7) // 驱动或设备错误


// status register bits, from qemu virtio_config.h
// #define VIRTIO_CONFIG_S_ACKNOWLEDGE 1
// #define VIRTIO_CONFIG_S_DRIVER 2
// #define VIRTIO_CONFIG_S_DRIVER_OK 4
// #define VIRTIO_CONFIG_S_FEATURES_OK 8

// device feature bits
#define VIRTIO_BLK_F_RO 5          /* Disk is read-only */
#define VIRTIO_BLK_F_SCSI 7        /* Supports scsi command passthru */
#define VIRTIO_BLK_F_CONFIG_WCE 11 /* Writeback mode available in config */
#define VIRTIO_BLK_F_MQ 12         /* support more than one vq */
#define VIRTIO_F_ANY_LAYOUT 27
#define VIRTIO_RING_F_INDIRECT_DESC 28
#define VIRTIO_RING_F_EVENT_IDX 29


// end



/* 描述符 flags 字段的位定义 */
#define VRING_DESC_F_NEXT       (1 << 0)  // 位0：链式描述符标志
#define VRING_DESC_F_WRITE     (1 << 1)  // 位1：数据方向标志
#define VRING_DESC_F_INDIRECT  (1 << 2)  // 位2：间接描述符标志

// 内存映射I/O基址（假设已映射到虚拟地址空间）
#define virtio_blk_base 0x10001000

static inline void memory_barrier()
{
    asm volatile ("fence w, w" ::: "memory");
}



void virtio_blk_init();
void virtio_submit_request(struct disk_request* req);



uint8_t virtio_get_status();

constexpr uint64 VIRTIO_F_VERSION_1 = (1ULL << 32); // VirtIO 1.0+ 设备