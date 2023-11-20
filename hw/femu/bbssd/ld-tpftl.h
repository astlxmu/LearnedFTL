
#ifndef _L_FEMU_FTL_H
#define _L_FEMU_FTL_H
#include "../nvme.h"


#define INVALID_PPA     (~(0ULL))
#define INVALID_LPN     (~(0ULL))
#define UNMAPPED_PPA    (~(0ULL))


#define CMT_HASH_SIZE (24593ULL)
#define TP_HASH_SIZE (24593ULL)
#define ENT_PER_TP (2048ULL)
#define GC_THRESH 5


enum {
    NAND_READ =  0,
    NAND_WRITE = 1,
    NAND_ERASE = 2,

    NAND_READ_LATENCY = 40000,
    NAND_PROG_LATENCY =  200000,
    NAND_ERASE_LATENCY = 2000000,
};

enum {
    CLEAN = 0,
    DIRTY = 1
};

enum {
    HEAD = 0,
    TAIL = 1
};

enum {
    USER_IO = 0,
    GC_IO = 1,
};

enum {
    GTD = 0,
    DATA = 1,
    UNUSED = 2,
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
    FEMU_PRINT_STAT = 9,
    FEMU_MODEL = 10,
    FEMU_MODEL_UNUSE = 11
};


#define BLK_BITS    (16)
#define PG_BITS     (16)
#define SEC_BITS    (8)
#define PL_BITS     (8)
#define LUN_BITS    (8)
#define CH_BITS     (7)

// #define CMT_NUM 16*1024
// #define CMT_NUM 4*1024
#define MAX_INTERVALS 8         // ! 模型参数：一个模型中包含几个段
#define INTERVAL_NUM 60         // ! 模型参数：忘了，没啥用应该，后面没用到
#define TRAIN_THRESHOLD 30      // ! 模型参数：对于整个模型，当有多少有效数据时进行模型训练

typedef struct lr_breakpoint {
    float w;
    float b;
    int key;
    int valid_cnt;
}lr_breakpoint;

typedef struct lr_node {

    lr_breakpoint brks[MAX_INTERVALS];
    uint64_t start_lpn;
    uint64_t start_ppa;
    uint8_t u;
    uint8_t less;
    float success_ratio;
}lr_node;

typedef struct cmt_entry {
    uint64_t lpn;
    uint64_t ppn;
    int dirty;
    // int hotness;
    QTAILQ_ENTRY(cmt_entry) entry;
    bool prefetch;
    uint64_t next_avail_time;
    struct cmt_entry *next;    /* for hash */
} cmt_entry;

typedef struct TPnode {
    uint64_t tvpn;
    int cmt_entry_cnt;
    // double hotness;  /* The paper didn't explain how to operate and set hotness */
    QTAILQ_ENTRY(TPnode) entry;
    //QTAIQ_HEAD 为热度最低的
    QTAILQ_HEAD(cmt_entry_list, cmt_entry) cmt_entry_list;
    struct TPnode *next;   /* for hash */
    short exist_ent[ENT_PER_TP];
} TPnode;


typedef struct hash_table {
    cmt_entry *cmt_table[CMT_HASH_SIZE];
    TPnode *tp_table[TP_HASH_SIZE];
}hash_table;

struct cmt_mgmt {
    cmt_entry *cmt_entries;
    QTAILQ_HEAD(free_cmt_entry_list, cmt_entry) free_cmt_entry_list;
    //QTAIQ_HEAD 为热度最高的
    QTAILQ_HEAD(TPnode_list, TPnode) TPnode_list;
    int tt_TPnodes;
    int tt_entries;
    int free_cmt_entry_cnt;
    int used_cmt_entry_cnt;
    // use for selective prefetching;
    int counter;
    struct hash_table ht;
};

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


    //==================modified====================
    int pg_size;
    int addr_size;
    int addr_per_pg;
    int tt_trans_pgs; 
    int tt_line_wps;
    int trans_per_line;

    int ents_per_pg;
    int tt_cmt_size;
    int tt_gtd_size;


    // * the virtual ppn params
    int chn_per_lun;
    int chn_per_pl;
    int chn_per_pg;
    int chn_per_blk;

    bool enable_request_prefetch;
    bool enable_select_prefetch;

};

typedef struct line {
    int id;  /* line id, the same as corresponding block id */
    int ipc; /* invalid page count in this line */
    int vpc; /* valid page count in this line */
    int type;

    // * identify how many entries remain in this line
    int rest;
    
    QTAILQ_ENTRY(line) entry; /* in either {free,victim,full} list */
    /* position in the priority queue for victim lines */
    size_t                  pos;
} line;

// *  modify ====================
struct wp_lines{
    struct line *line;
    struct wp_lines *next;
};

/* wp: record next write addr */
struct write_pointer {
    struct line *curline;
    struct wp_lines *wpl;
    int vic_cnt;
    int ch;
    int lun;
    int pg;
    int blk;
    int pl;
    int id;

    // TODO: how many lines this wpp invade
    int invade_lines;
};

struct line_statistic {
    int rest;
};

struct line_mgmt {
    struct line *lines;
    /* free line list, we only need to maintain a list of blk numbers */
    QTAILQ_HEAD(free_line_list, line) free_line_list;
    pqueue_t *victim_line_pq;
    //QTAILQ_HEAD(victim_line_list, line) victim_line_list;
    QTAILQ_HEAD(full_line_list, line) full_line_list;
    QTAILQ_HEAD(victim_list, line) victim_list;
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


// a simple hash table for DFTL
struct ht {
    int max_depth;
    int left_num;
    int global_version;
    struct ppa *entry;
    uint64_t *lpns;
    uint32_t *version;
    uint8_t *dirty;

};

struct statistics {
    uint64_t cmt_hit_cnt;
    uint64_t cmt_miss_cnt;
    double cmt_hit_ratio;
    uint64_t access_cnt;
    uint64_t model_hit_num;
    uint64_t model_use_num;
    uint64_t model_out_range;
    uint64_t max_lpn;
    uint64_t min_lpn;
    uint64_t req_num;
    long double average_lat;
    uint64_t gc_times;
    uint64_t write_num;
    uint64_t line_gc_times[512];
    uint64_t wp_victims[512];
    uint64_t trans_wp_gc_times;
    uint64_t line_wp_gc_times;
    long long calculate_time;
    long long sort_time;
    long long predict_time;
    long long GC_time;
    long long write_time;
    long long read_time;
    long long model_training_nums;
    int gc_cnt;
    // uint64_t max_read_lpn;
    // uint64_t min_read_lpn;
    // uint64_t max_write_lpn;
    // uint64_t min_write_lpn;
    long double read_joule;
    long double write_joule;
    long double erase_joule;
    long double joule;
};

struct ssd {
    char *ssdname;
    struct ssdparams sp;
    struct ssd_channel *ch;
    struct ppa *maptbl; /* page level mapping table */
    uint64_t *rmap;     /* reverse mapptbl, assume it's stored in OOB */
    uint8_t *bitmaps;

    struct line_mgmt lm;

    //===============modified=======================
    struct ppa *gtd;    // global translation blocks
    struct write_pointer *gtd_wps;  // for every 32 translation pags, there is a write pointer
    uint64_t *gtd_usage;

    // * the cmt management
    struct cmt_mgmt cm;

    struct write_pointer trans_wp; // the write pointer for writing translation pages
    struct lr_node *lr_nodes;  // the linear regression model
    struct ht cmt;    // current mapping table
    uint64_t num_trans_write;
    uint64_t num_data_write;
    uint64_t num_data_read;

    /* lockless ring for communication with NVMe IO thread */
    struct rte_ring **to_ftl;
    struct rte_ring **to_poller;
    bool *dataplane_started_ptr;
    
    bool model_used;
    uint8_t *valid_lines;

    struct statistics stat;
    QemuThread ftl_thread;

    // * the line-write_pointer mapping
    struct write_pointer **line2write_pointer;

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