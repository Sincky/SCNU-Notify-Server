﻿========================================================================
    控制台应用程序：SCNU-Notify-Server 项目概述
========================================================================

main.cpp
    入口主程序

server.cpp \ server.h
    服务框架，实现整个流程
    
smtp.cpp \ smtp.h
    用来连接发信服务器和发送邮件（socket）
    
http.cpp \ http.h
    实现网页源代码的读取（socket）

base64.cpp \ base64.h
    编码转换,发信用

conf.ini
    配置文件，实现动态添加订阅人和其他信息

smtp.cbp
    codeblocks项目配置文件