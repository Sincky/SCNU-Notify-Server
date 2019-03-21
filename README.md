# SCNU-Notify-Server
SCNU专用通知服务器，主要是通过socket来抓取[共青团委员会](http://youth.scnu.edu.cn/qy_tongzhigonggao/)的通知公告，检测是否有新的通知，然后通知到订阅人。

# 环境和依赖

系统环境：linux 

IDE：CodeBlocks

C++版本：C++11 或者更高

GCC版本：GCC4.9 或者更高

lib依赖：socket

# 其他说明

conf.ini是发信人和订阅人的信息配置文件，可不用重新编译来实现动态更新发信人和订阅人信息

建议每6个小时读取一次，太快会被封ip（实测被封过，惨），放到服务器上运行即可。
