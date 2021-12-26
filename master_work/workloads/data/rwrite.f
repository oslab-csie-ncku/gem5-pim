set $dir=/nova
set $filesize=2g
set $iosize=1m
set $bytes=8 # 1 ~ 1024 (m)

debug 0

define file name=bigfile1,path=$dir,size=$filesize,prealloc

define process name=filewriter,instances=1
{
  thread name=filewriterthread,memsize=10m,instances=1
  {
    flowop write name=write-file,filename=bigfile1,random,iosize=$iosize,iters=$bytes
    flowop finishoncount name=finish,value=1,target=write-file
  }
}

create files

run
