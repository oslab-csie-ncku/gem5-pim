set $dir=/nova
set $iosize=1m
set $bytes=8 # 1 ~ 1024 (m)

debug 0

define file name=bigfile,path=$dir,size=0,prealloc

define process name=filewriter,instances=1
{
  thread name=filewriterthread,memsize=10m,instances=1
  {
    flowop write name=write-file,filename=bigfile,iosize=$iosize,iters=$bytes
    flowop finishoncount name=finish,value=1,target=write-file
  }
}

create files

run
