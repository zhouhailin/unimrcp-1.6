


### 第一步 配置编译UniMRCP Server

本次示例的UniMRCP Server在CentOS 7中进行源码编译安装，
1.下载 [unimrcp-1.6 源码](https://github.com/peter158/unimrcp-1.6)：

```shell
cd /opt
git clone -b unimrcp-1.6 https://github.com/peter158/unimrcp-1.6.git
```

2.编译准备环境


下载 [unimrcp-deps-1-6-0](https://www.unimrcp.org/project/release-view/unimrcp-deps-1-6-0/unimrcp-deps-1-6-0-zip)
```shell
./build-dep-libs.sh
```
>注：1.过程中需要输入两次y，并确认

3.编译安装unimrcp

```shell
cd unimrcp-1.6.0
./bootstrap
## 如果不能自动检测apr，apr-util,请在configure中增加 option：--with-apr=/path/apr --with-apr-util=/path/apr-util/
## apr， apr-util由./build-dep-libs.sh 生成
./configure
make
make install
```
即可在/usr/local/中看到安装好的unimrcp。

4.测试运行

```shell
cd /usr/local/unimrcp/bin
./unimrcpserver -o 3
```

可以使用client进行验证

```shell
cd /usr/local/unimrcp/bin
./unimrcpclient
>help
usage:

- run [app_name] [profile_name] (run demo application)
       app_name is one of 'synth', 'recog', 'bypass', 'discover'
       profile_name is one of 'uni2', 'uni1', ...

       examples: 
           run synth
           run recog
           run synth uni1
           run recog uni1

- loglevel [level] (set loglevel, one of 0,1...7)

- quit, exit
```


注：替换unimrcp的VAD模块


unimrcp vad 模块voice activity dector一直认为比较粗暴，而且unimrcp的社区也很久没有更新了。使用原始unimrcp如果只是用来做Demo演示，通过手动调整参数，还是可以的。但是距离生产环境，还是有很远的一段路。故可以替换成webrtc的vad模块。

参考下面链接，本项目已经替换成webrtc的vad模块，无需再修改。
https://www.cnblogs.com/damizhou/p/11323394.html

代码实现来源：[UniMRCP-with-freeswitch](https://github.com/reSipWebRTC/UniMRCP-with-freeswitch
)
### 第二步 配置与验证

#### 配置

配置FreeSWITCH

我们需要将处理用户语音呼入的FreeSWITCH与向xfyun engine发请求的unimrcp server两者连接起来。

1.配置unimrcp模块并自动加载；

```shell
# 编辑/usr/local/src/freeswitch/modules.conf文件，找到要安装的模块，去掉前面的注释符号#
cd /usr/local/src/freeswitch
vim modules.conf
#asr_tts/mod_unimrcp
asr_tts/mod_unimrcp

# 执行make mod_xxx-install命令，这样就编译相应模块，并把编译后的动态库安装的/usr/local/freeswitch/mod目录下
make mod_unimrcp-install

# 编辑/usr/local/freeswitch/conf/autoload_configs/modules.conf.xml，去掉注释符号，如果没有发现对应模块，则添加
<load module="mod_unimrcp"/>
```

2.设置profile文件与conf文件；

在/usr/local/freeswitch/conf/mrcp_profiles目录新建unimrcpserver-mrcp-v2.xml配置文件：

```xml
<include>
  <!-- UniMRCP Server MRCPv2 -->
  <!-- 后面我们使用该配置文件，均使用 name 作为唯一标识，而不是文件名 -->
  <profile name="unimrcpserver-mrcp2" version="2">
    <!-- MRCP 服务器地址 -->
    <param name="server-ip" value="192.168.1.23"/>
    <!-- MRCP SIP 端口号 -->
    <param name="server-port" value="8060"/>

    <!-- FreeSWITCH IP、端口以及 SIP 传输方式 -->
    <param name="client-ip" value="192.168.1.24" />
    <param name="client-port" value="5069"/>
    <param name="sip-transport" value="udp"/>

    <!--param name="rtp-ext-ip" value="auto"/-->
    <param name="rtp-ip" value="192.168.1.24"/>
    <param name="rtp-port-min" value="4000"/>
    <param name="rtp-port-max" value="5000"/>
    <param name="codecs" value="PCMU PCMA L16/96/8000"/>

    <!-- Add any default MRCP params for SPEAK requests here -->
    <synthparams>
    </synthparams>

    <!-- Add any default MRCP params for RECOGNIZE requests here -->
    <recogparams>
      <!--param name="start-input-timers" value="false"/-->
    </recogparams>
  </profile>
</include>
```

配置/usr/local/freeswitch/conf/autoload_configs/unimrcp.conf.xml文件：

```xml
<configuration name="unimrcp.conf" description="UniMRCP Client">
  <settings>
    <!-- UniMRCP profile to use for TTS -->
    <param name="default-tts-profile" value="unimrcpserver-mrcp2"/>
    <!-- UniMRCP profile to use for ASR -->
    <param name="default-asr-profile" value="unimrcpserver-mrcp2"/>
    <!-- UniMRCP logging level to appear in freeswitch.log.  Options are:
         EMERGENCY|ALERT|CRITICAL|ERROR|WARNING|NOTICE|INFO|DEBUG -->
    <param name="log-level" value="DEBUG"/>
    <!-- Enable events for profile creation, open, and close -->
    <param name="enable-profile-events" value="false"/>

    <param name="max-connection-count" value="100"/>
    <param name="offer-new-connection" value="1"/>
    <param name="request-timeout" value="3000"/>
  </settings>

  <profiles>
    <X-PRE-PROCESS cmd="include" data="../mrcp_profiles/*.xml"/>
  </profiles>

</configuration>
```
>注：1.unimrcpserver-mrcp-v2.xml中server-ip为unimrcpserver启动的主机ip；2.client-ip和rtp-ip为FreeSWITCH启动的主机，client-port仕FreeSWITCH作为客户端访问unimrcpserver的端口，手机作为客户端访问的FreeSWITCH端口默认为5060，两者不同；3.unimrcpserver-mrcp-v2.xml中的profile name应和unimrcp.conf.xml中的default-tts-profile与default-ars-profile的value一致（有些文档的分析中称mrcp_profiles中的xml文件名也必须和这两者一致，实际上是非必须的）。

> **Attenion: unimrcpserver 和 freeswitch 部署在同一个网段很重要，最好部署测试的时候在同一台物理机器上进行**

3.配置IVR与脚本。

在/usr/local/freeswitch/conf/dialplan/default.xml里新增如下配置：

```xml
<extension name="unimrcp">
    <condition field="destination_number" expression="^5001$">
          <action application="answer"/>
        <action application="lua" data="names.lua"/>
    </condition>
</extension>
```

在/usr/local/freeswitch/scripts目录下新增names.lua脚本：

```lua
session:answer()

--freeswitch.consoleLog("INFO", "Called extension is '".. argv[1]"'\n")
welcome = "ivr/ivr-welcome_to_freeswitch.wav"
menu = "ivr/ivr-this_ivr_will_let_you_test_features.wav"
--
grammar = "hello"
no_input_timeout = 80000
recognition_timeout = 80000
confidence_threshold = 0.2
--
session:streamFile(welcome)
--freeswitch.consoleLog("INFO", "Prompt file is \n")

tryagain = 1
 while (tryagain == 1) do
 --
       session:execute("play_and_detect_speech",menu .. "detect:unimrcp {start-input-timers=false,no-input-timeout=" .. no_input_timeout .. ",recognition-timeout=" .. recognition_timeout .. "}" .. grammar)
       xml = session:getVariable('detect_speech_result')
 --
       if (xml == nil) then
               freeswitch.consoleLog("CRIT","Result is 'nil'\n")
               tryagain = 0
       else
               freeswitch.consoleLog("CRIT","Result is '" .. xml .. "'\n")
               tryagain = 0
    end
end
 --
 -- put logic to forward call here
 --
 session:sleep(250)
 session:set_tts_params("unimrcp", "xiaofang");
 session:speak("今天天气不错啊");
 session:hangup()
```

我们需要在/usr/local/freeswitch/grammar目录新增hello.gram语法文件，可以为空语法文件须满足语音识别语法规范1.0标准（简称 [SRGS1.0](https://www.w3.org/TR/speech-grammar/)），该语法文件 ASR 引擎在进行识别时可以使用。

```xml
<?xml version="1.0" encoding="utf-8" ?>
<grammar version="1.0" xml:lang="zh-cn" root="Menu" tag-format="semantics/1.0"
  　　　　xmlns=http://www.w3.org/2001/06/grammar
　　　　xmlns:sapi="http://schemas.microsoft.com/Speech/2002/06/SRGSExtensions"><!- 这些都是必不可少的-->
  <rule id="city" scope="public">
    <one-of>     <!-- 匹配其中一个短语-->
      <item>北京</item>
      <item>上海</item>
    </one-of>
  </rule>
  <rule id="cross" scope="public">
    <one-of>
      <item>到</item>
      <item>至</item>
      <item>飞往</item>
    </one-of>
  </rule>
  <rule id="Menu" scope="public">
    <item>
      <ruleref uri="#date"/>         <!--指定关联的其他规则的节点-->
      <tag>out.date = reles.latest();</tag>
    </item>
    <item repeat="0-1">从</item>    <!--显示1次或0次-->
    <item>
      <ruleref uri="#city"/>
      <tag>out.city = rulels.latest();</tag>
    </item>
    <item>
      <ruleref uri="#cross"/>
      <tag>out.cross = rulels.latest();</tag>
    </item>
    <item>
      <ruleref uri="#city"/>
      <tag>out.city = rulels.latest();</tag>
    </item>
  </rule>
</grammar>
```

>注：lua脚本中，”play_and_detect_speech” 调用了 ASR 服务，”speak” 调用了 TTS 服务。[配置启动中遇到问题](https://www.jianshu.com/p/6aa2140937b2)。

#### SIP客户端
- [Android Linphone](http://www.linphone.org/sites/default/files/linphone-latest.apk)
- [iOS 22Call SIP](https://apps.apple.com/app/id1510212085)



###参考
==========

Website:
   http://www.unimrcp.org

Downloads:
   http://www.unimrcp.org/downloads

Documentation:
   http://www.unimrcp.org/documentation

GitHub:
   https://github.com/unispeech/unimrcp

Issue Tracker:
   https://github.com/unispeech/unimrcp/issues

Discussion Group:
   https://groups.google.com/group/unimrcp

Source Changes:
   https://github.com/unispeech/unimrcp/commits/master
   https://groups.google.com/group/unimrcp-commits


LICENSING
=========

UniMRCP is licensed under the terms of the Apache License 2.0.
See the file "LICENSE" for more information.

Copyright 2008 - 2018 Arsen Chaloyan

