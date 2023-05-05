
# 基于ESP8266,写一个Arduino程序

## 要求：
1. 让用户在网页上输入三个变量：

`string blynk_token;`

`string blynk_server;`

`string blynk_port;`

- `blynk_token`可输入内容：Blynk 应用程序的身份验证令牌，通常是一长串随机生成的字符。（例如：dkQNyU5ddCLIsLa2xv6IPKy_IlxKkZsd）、
- `blynk_server`Blynk 服务器的主机名或 IP 地址。（要求能识别域名和ip例如：chrisxs.com，或者IP地址）、
- `blynk_port`Blynk 服务器使用的端口号，通常是 8080，只能是数字

2. 增加保存按钮，让用户输入并保存这三个变量

3. 增加清除配置按钮，让用户可以请出存储这个配置的文件

4. 如果已经储存配置的文件，则把这三个值在网页中显示出来

## 程序流程：

1. 用户在网页上输入这三个变量

2. 点击网页的“保存Blynk配置后”，提示用户保存成功，把变量新建保存成名为：`blynk_config.json`的json文件到ESP8266的FS中，并重启esp8266。然后使用：`Blynk.config()`，调用他们

3. 如果用户点击清除配置按钮，则删除`blynk_config.json`,然后重启ESP8266

## 错误处理

1. 三个变量的值可以为空，并都可以保存
2. 除非值为空，否则要求校验所输入的值符不符合：

`blynk_token`必须是字符

`blynk_server`必须是域名或者IP地址

`blynk_port`必须是数字

3. 用户输入后保存，先打印这三个标量，并从`blynk_config.json`读取出来核对有没有输入正确

