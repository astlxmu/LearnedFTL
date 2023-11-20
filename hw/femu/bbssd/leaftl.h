#ifndef __FEMU_DFTL_H
#define __FEMU_DFTL_H

#include "../nvme.h"

#define INVALID_PPA     (~(0ULL))
#define INVALID_LPN     (~(0ULL))
#define UNMAPPED_PPA    (~(0ULL))

#define CMT_HASH_SIZE (24593ULL)

enum {
    NAND_READ =  0,
    NAND_WRITE = 1,
    NAND_ERASE = 2,

    NAND_READ_LATENCY = 40000,
    NAND_PROG_LATENCY = 200000,
    NAND_ERASE_LATENCY = 2000000,
};

enum {
    USER_IO = 0,
    GC_IO = 1,
};

enum {
    SEC_FREE = 0,
    SEC_INVALID = 1,
    SEC_VALID = 2,

    PG_FREE = 0,
    PG_INVALID = 1,
    PG_VALID = 2
};

enum {
    FEMU_ENABLE_GC_DELAY = 1,
    FEMU_DISABLE_GC_DELAY = 2,

    FEMU_ENABLE_DELAY_EMU = 3,
    FEMU_DISABLE_DELAY_EMU = 4,

    FEMU_RESET_ACCT = 5,
    FEMU_ENABLE_LOG = 6,
    FEMU_DISABLE_LOG = 7,

    FEMU_RESET_STAT = 8,
    FEMU_PRINT_STAT = 9
};

enum {
    CLEAN = 0,
    DIRTY = 1
};

enum {
    NONE = 0,
    DATA = 1,
    TRANS = 2
};

#define BLK_BITS    (16)
#define PG_BITS     (16)
#define SEC_BITS    (8)
#define PL_BITS     (8)
#define LUN_BITS    (8)
#define CH_BITS     (7)

/* describe a physical page addr */
struct ppa {
    union {
        struct {
            uint64_t blk : BLK_BITS;
            uint64_t pg  : PG_BITS;
            uint64_t sec : SEC_BITS;
            uint64_t pl  : PL_BITS;
            uint64_t lun : LUN_BITS;
            uint64_t ch  : CH_BITS;
            uint64_t rsv : 1;
        } g;

        uint64_t ppa;
    };
};

typedef int nand_sec_status_t;

struct nand_page {
    nand_sec_status_t *sec;
    int nsecs;
    int status;
};

struct nand_block {
    struct nand_page *pg;
    int npgs;
    int ipc; /* invalid page count */
    int vpc; /* valid page count */
    int erase_cnt;
    int wp; /* current write pointer */
};

struct nand_plane {
    struct nand_block *blk;
    int nblks;
};

struct nand_lun {
    struct nand_plane *pl;
    int npls;
    uint64_t next_lun_avail_time;
    bool busy;
    uint64_t gc_endtime;
};

struct ssd_channel {
    struct nand_lun *lun;
    int nluns;
    uint64_t next_ch_avail_time;
    bool busy;
    uint64_t gc_endtime;
};

struct ssdparams {
    int secsz;        /* sector size in bytes */
    int secs_per_pg;  /* # of sectors per page */
    int pgs_per_blk;  /* # of NAND pages per block */
    int blks_per_pl;  /* # of blocks per plane */
    int pls_per_lun;  /* # of planes per LUN (Die) */
    int luns_per_ch;  /* # of LUNs per channel */
    int nchs;         /* # of channels in the SSD */

    int pg_rd_lat;    /* NAND page read latency in nanoseconds */
    int pg_wr_lat;    /* NAND page program latency in nanoseconds */
    int blk_er_lat;   /* NAND block erase latency in nanoseconds */
    int ch_xfer_lat;  /* channel transfer latency for one page in nanoseconds
                       * this defines the channel bandwith
                       */

    double gc_thres_pcent;
    int gc_thres_lines;
    double gc_thres_pcent_high;
    int gc_thres_lines_high;
    bool enable_gc_delay;

    /* below are all calculated values */
    int secs_per_blk; /* # of sectors per block */
    int secs_per_pl;  /* # of sectors per plane */
    int secs_per_lun; /* # of sectors per LUN */
    int secs_per_ch;  /* # of sectors per channel */
    int tt_secs;      /* # of sectors in the SSD */

    int pgs_per_pl;   /* # of pages per plane */
    int pgs_per_lun;  /* # of pages per LUN (Die) */
    int pgs_per_ch;   /* # of pages per channel */
    int tt_pgs;       /* total # of pages in the SSD */

    int blks_per_lun; /* # of blocks per LUN */
    int blks_per_ch;  /* # of blocks per channel */
    int tt_blks;      /* total # of blocks in the SSD */

    int secs_per_line;
    int pgs_per_line;
    int blks_per_line;
    int tt_lines;

    int pls_per_ch;   /* # of planes per channel */
    int tt_pls;       /* total # of planes in the SSD */

    int tt_luns;      /* total # of LUNs in the SSD */

    int ents_per_pg;
    int tt_cmt_size;
    int tt_gtd_size;

    int buffer_size;

    int chn_per_lun;
    int chn_per_pl;
    int chn_per_pg;
    int chn_per_blk;
    
};

typedef struct line {
    int id;  /* line id, the same as corresponding block id */
    int ipc; /* invalid page count in this line */
    int vpc; /* valid page count in this line */
    QTAILQ_ENTRY(line) entry; /* in either {free,victim,full} list */
    /* position in the priority queue for victim lines */
    size_t                  pos;
    int type;
} line;

/* wp: record next write addr */
struct write_pointer {
    struct line *curline;
    int ch;
    int lun;
    int pg;
    int blk;
    int pl;
};

/* wp: record next translation page write addr */
struct trans_write_pointer {
    struct line *curline;
    int ch;
    int lun;
    int pg;
    int blk;
    int pl;
};

struct line_mgmt {
    struct line *lines;
    /* free line list, we only need to maintain a list of blk numbers */
    QTAILQ_HEAD(free_line_list, line) free_line_list;
    pqueue_t *victim_line_pq;
    //QTAILQ_HEAD(victim_line_list, line) victim_line_list;
    QTAILQ_HEAD(full_line_list, line) full_line_list;
    int tt_lines;
    int free_line_cnt;
    int victim_line_cnt;
    int full_line_cnt;
};

struct nand_cmd {
    int type;
    int cmd;
    int64_t stime; /* Coperd: request arrival time */
};

typedef struct simple_segment {
    uint64_t slpa;
    int len;
    uint8_t is_accurate;
    float k;
    uint64_t inter;
    struct simple_segment *next;
    
}simple_segment;


typedef struct cmt_entry {
    uint64_t lpn;
    simple_segment sm;
    int dirty;
    QTAILQ_ENTRY(cmt_entry) entry;
    struct cmt_entry *next;    /* for hash */
} cmt_entry;

typedef struct hash_table {
    cmt_entry *cmt_table[CMT_HASH_SIZE];
}hash_table;

struct cmt_mgmt {
    cmt_entry *cmt_entries;
    QTAILQ_HEAD(free_cmt_entry_list, cmt_entry) free_cmt_entry_list;
    //QTAIQ_HEAD 为热度最高的
    QTAILQ_HEAD(cmt_entry_list, cmt_entry) cmt_entry_list;
    int tt_entries;
    int free_cmt_entry_cnt;
    int used_cmt_entry_cnt;
    struct hash_table ht;
};

struct statistics {
    uint64_t cmt_hit_cnt;
    uint64_t cmt_miss_cnt;
    double cmt_hit_ratio;
    uint64_t access_cnt;
    uint64_t write_cnt;
    uint64_t model_hit;
    double model_hit_ratio;
    int single_read;
    int double_read;
    int triple_read;
    int wa_cnt;
    int gc_cnt;
    long double read_joule;
    long double write_joule;
    long double erase_joule;
    long double joule;
};


typedef struct segment_param {
    int sgement_cnt;
    int page_total_cnt;
    int mapping_per_page;
    int threshold;
    int buffer_len;
}segment_param;







typedef struct segment {
    int segment_no;
    struct simple_segment* segment_head;
    int seg_cnt;
    bool is_valid;
    struct segment* next;
}segment;


typedef struct level_seg_mgmt{
    segment **level_seg;
    int valid_cnt;

    struct level_seg_mgmt* next;
}level_seg_mgmt;



typedef struct all_seg_mgmt {
    level_seg_mgmt *levels;
    int *level_cnt;

}all_seg_mgmt;

typedef struct cr_bm {
    uint64_t *record;
    struct cr_bm* next;
    int cnt;
    simple_segment* to_sm;
}cr_bm;

typedef struct conflict_resolution_buffer {

    cr_bm **crb_unit;
    // uint8_t **CR_bitmap;
}conflict_resolution_buffer;


typedef struct group_segments_mgmt {
    segment** level_seg;
    int *seg_valid;

}group_segments_mgmt;


typedef struct learned_mgmt {
    segment_param *sp;
    all_seg_mgmt *all_segs;
    conflict_resolution_buffer *crb;

    uint8_t *bitmap; // ! 辅助compact


}learned_mgmt;



struct ssd {
    char *ssdname;
    struct ssdparams sp;
    struct ssd_channel *ch;
    struct ppa *maptbl; /* page level mapping table */
    uint64_t *rmap;     /* reverse mapptbl, assume it's stored in OOB */
    struct write_pointer wp;
    struct line_mgmt lm;

    struct cmt_mgmt cm;
    struct ppa *gtd;
    struct trans_write_pointer twp;

    /* lockless ring for communication with NVMe IO thread */
    struct rte_ring **to_ftl;
    struct rte_ring **to_poller;
    bool *dataplane_started_ptr;
    QemuThread ftl_thread;

    struct statistics stat;

    // ! Modified
    learned_mgmt* lem;
    uint64_t *buffer_lpns;
    int buffer_pointer;
    bool start_write;
};

void ssd_init(FemuCtrl *n);
void count_segments(struct ssd* ssd);

#ifdef FEMU_DEBUG_FTL
#define ftl_debug(fmt, ...) \
    do { printf("[FEMU] FTL-Dbg: " fmt, ## __VA_ARGS__); } while (0)
#else
#define ftl_debug(fmt, ...) \
    do { } while (0)
#endif

#define ftl_err(fmt, ...) \
    do { fprintf(stderr, "[FEMU] FTL-Err: " fmt, ## __VA_ARGS__); } while (0)

#define ftl_log(fmt, ...) \
    do { printf("[FEMU] FTL-Log: " fmt, ## __VA_ARGS__); } while (0)


/* FEMU assert() */
#ifdef FEMU_DEBUG_FTL
#define ftl_assert(expression) assert(expression)
#else
#define ftl_assert(expression)
#endif

#endif
