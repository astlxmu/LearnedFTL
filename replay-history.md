sudo nvme list
    2  sudo nvme lnvm id-ns /dev/nvme0n1
    3  sudo nvme lnvm create -t pblk -d nvme0n1 -n tgt0 -b 0 -e 63
    4  dmesg
    5  sudo modprobe pblk
    6  dmesg
    7  sudo nvme lnvm create -t pblk -d nvme0n1 -n tgt0 -b 0 -e 63
    8  sudo mkfs.ext4 /dev/tgt0 
    9  sudo mount /dev/tgt0 /mnt/
   10  df -h
   11  u
   12  pwd
   13  dmesg
   14  reset
   15  u
   16  pwd
   17  lsblk
   18  u
   19  pwd
   20  ls
   21  whoami
   22  pwd
   23  ls
   24  pwd
   25  git log
   26  u
   27  pwd
   28  sudo rm -rf /var/log/kern.log 
   29  sudo rm -rf ~/.bash_history 
   30  v ~/.sudo_as_admin_successful 
   31  vim ~/.sudo_as_admin_successful
   32  sudo shutdown -h now
   33  sudo ./replayer/replayer /dev/nvnme0n1 systor_trace_new_125w out-leaf-125
   34  sudo ./replayer/replayer /dev/nvme0n1 systor_trace_new_125w out-leaf-125
   35  top
   36  git clone https://github.com/facebook/rocksdb.git
   37  ls
   38  cd rocksdb/
   39  ls
   40  make db_bench
   41  ls
   42  sudo bash ../filebench.sh 
   43  df -h
   44  sudo ./db_bench -db=/tmp benchmarks=fillseq --num=10000000 --key_size=16 --value_size=4096
   45  cd third-party/
   46  ls
   47  sudo apt-get install libsgflags-dev
   48  sudo apt-cache search gflags
   49  sudo apt-get install libgflags-dev 
   50  cd ..
   51  sudo ./db_bench -db=/tmp benchmarks=fillseq --num=10000000 --key_size=16 --value_size=4096
   52  make db_bench
   53  sudo ./db_bench -db=/tmp benchmarks=fillseq --num=10000000 --key_size=16 --value_size=4096
   54  rm -f db_bench 
   55  sudo ./db_bench -db=/tmp benchmarks=fillseq --num=10000000 --key_size=16 --value_size=4096
   56  make db_bench
   57  sudo ./db_bench -db=/tmp benchmarks=fillseq --num=10000000 --key_size=16 --value_size=4096
   58  sudo find / -name libgflags.so
   59  sudo find / -name gflags
   60  cd /usr/include/gflags/
   61  ;s
   62  ls
   63  cd ..
   64  cd /home/femu/
   65  ls
   66  rm -rf rocksdb/
   67  git clone https://github.com/facebook/rocksdb.git
   68  cd rocksdb/
   69  ls
   70  make db_bench
   71  sudo ./db_bench -db=/tmp benchmarks=fillseq --num=10000000 --key_size=16 --value_size=4096
   72  sudo apt-get install snappy
   73  sudo apt-cache search snappy
   74  sudo apt-get install libsnappy-dev
   75  sudo ./db_bench -db=/tmp benchmarks=fillseq --num=10000000 --key_size=16 --value_size=4096
   76  sudo apt-get install libgflags-dev libsnappy-dev zlib1g-dev libbz2-dev liblz4-dev libzstd-dev
   77  ls
   78  sudo ./db_bench -db=/tmp benchmarks=fillseq --num=10000000 --key_size=16 --value_size=4096
   79  cd ..
   80  cd rocksdb/
   81  make clean
   82  make db_bench
   83  \
   84  sudo ./db_bench -db=/tmp benchmarks=fillseq --num=10000000 --key_size=16 --value_size=4096
   85  sudo shutdown -h now
   86  ls
   87  cd rocksdb/
   88  top
   89  ls
   90  l
   91  ls
   92  df -h
   93  ls
   94  ls -l
   95  cd replayer
   96  ls
   97  cd ..
   98  ls
   99  nano filebench.sh
  100  ls
  101  history