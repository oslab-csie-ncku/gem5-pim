set $dir=/nova
set $meandirwidth=0
set $nfiles=30000
set $count=30000

debug 0

define fileset name=fset,path=$dir,entries=$nfiles,filesize=0,dirwidth=$meandirwidth,prealloc

define process name=filedelete,instances=1
{
  thread name=filedeletethread,memsize=10m,instances=1
  {
    flowop deletefile name=deletefile1,filesetname=fset
    flowop finishoncount name=finish,value=$count,target=deletefile1
  }
}

create files

run
