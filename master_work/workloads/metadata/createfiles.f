set $dir=/nova
set $meandirwidth=0
set $nfiles=30000
set $count=30000

debug 0

define fileset name=fset,path=$dir,entries=$nfiles,filesize=0,dirwidth=$meandirwidth

define process name=filecreate,instances=1
{
  thread name=filecreatethread,memsize=10m,instances=1
  {
    flowop createfile name=createfile1,filesetname=fset
    flowop closefile name=closefile1
    flowop finishoncount name=finish,value=$count,target=createfile1
  }
}

create files

run
