# RxAutoLogin

随便玩玩按键精灵写的热血江湖自动登录。

==**`若不信dll可自行编译源码/其他地方下载。被盗后果自负！！！`**==

==**`若不信dll可自行编译源码/其他地方下载。被盗后果自负！！！`**==

==**`若不信dll可自行编译源码/其他地方下载。被盗后果自负！！！`**==

## 环境

- win10
- 游戏版本 V23023
- 游戏分辨率 1280*720 (若不同影响鼠标点击，可自行修改)

## 使用

- 完全关闭按键精灵。

- 下载 [RxSocket.dll](searchLines/RXSocket.dll) 放到按键精灵安装目录下的 plugin 里。

- 下载 [RxAutoLogin.Q](RxAutoLogin.Q) 放到按键精灵安装目录下的 QMScript 目录下

- 启动按键精灵打开脚本。

  - 不知道 .Q 文件是否包含附件，若没有则下载 [attachment](attachment) 下的所有文件扔到脚本界面下的附件里。

    > dm.dll 为大漠插件 3.1233 免费版(可自行下载)；codeMap.txt 为验证码点阵。

  - 脚本属性

    - 脚本循环：循环 1 次
    - 勾选 "禁止重复运行"

- **运行方式任意选择：**

  1. 脚本编写界面 调试 -> 自定义界面

  2. 按键精灵主界面->我的脚本->选中脚本->点击右边的属性->自定义界面

  3. 发布按键小精灵使用。

     > 因为一直修修改改，所以没搞按键小精灵，可自行打包。
     >
     > 不提供 exe 请自行发布！

## 介绍

简单介绍下流程及部分控件功能

### 流程

- 界面初始化
  加载 ini 文件数据到界面

- 登录初始化

  - 初始化界面部分变量数据与基址

  - 初始化路径( launcher.exe、收费神器、免费神器)并保存，影响后续的自动更新等，必须！

  - 初始化维护等待

    若勾选[功能2-维护等待]维护日脚本需等到具体时间后再自动更新等，避免 17/SQ 未完成更新发布！

    > 注意：如勾选且选择周四 12 点，则 0-6 点启动脚本直接跳过，6-12 点启动脚本会等到 12 点后再继续，可取消勾选！

  - 初始化自动更新游戏

    若勾选[功能2-自动更新] 将进行下2步自动更新游戏

    - 尝试 3m 内去网址下载 zip 后台静默自动更新，若超时将进行下步
    - 尝试等待电二开服再启动 launcher.exe 10m 内更新，若再失败则脚本结束需手动更新后重启脚本！

  - 初始化收费&免费神器

    若已经打开则不做操作，若未开则自动更新神器后打开并隐藏。

  - 初始化子线程 10s 循环更新页面表格的金钱、等级、人参、强化、仓余列。

  - **主循环** 自动检测断线与账号登录。

    - shellExecute 启动游戏等待登录
    - 登录进线
    - 选角进游
    - 激活外挂
    - 开始挂机

### 部分控件

- 最右

  - 显示隐藏：选中表格某条数据后点击按钮
  - 一键显示/隐藏
  - 保存设置：保存了部分需要的页面值（不读内存、功能2全部、子线程 N 秒循环）
  - 收费挂：显示隐藏
  - 免费挂：显示隐藏

- 功能

  - 清空日志：清空左侧显示日志，默认 300 条也会自动清空
  - 开查线器：打开电五沉香放左下侧
  - 过验证码：选中表格某条数据后点击按钮
  - 更新仓余：需不勾"不读内存"后点击才更新
  - 是否定点：选中表格某条数据后点击按钮
  - 账号全选
  - 不读内存：影响表格的金钱、等级、人参、强化、仓余列的更新，勾选省 cpu。
  - 黑屏： 选中表格某条数据后点击按钮后黑屏/显示
  - 黑屏13： 黑屏会修改 d3d9.dll 五处位置，最少需要第 1、3 项可按键查字。选中表格某条数据后点击按钮黑屏13/显示13。

- 账号操作

  - 账号：**必须！** 为 ID 依据若存在则修改，不存在则新增。

  - 线路：以减号"-"分隔，可选多条线路。(会格式化保存去空、去非数字、去 0 )现是以沉香为例如普通线为3-...-16，若其他区自改 lineArray 与 lineArray2。大于服务器总线路数会在查线时自动排除！

  - 角色名： **必须！** 很多以此为判断依据。

  - 上线后： 含有"定"的可选但没用，脚本直接控制定点。开店没用过没写，有需要自写。

  - 备注： 随便写该账号待定事宜提醒自己。

  - 定点： 勾选实时生效！

  - 攻击距离：输入框失去焦点实时生效！范围 0-299，0 为还原默认。

  - 增改：以账号为依据若存在则修改，不存在则新增。

    > 补充：账号操作的增改及删除是实时更新界面表格与保存到文件的，无需其他操作。

- 功能2

  - 三浏览框分别输入对应路径( launcher.exe、收费神器、免费神器)，**必须！**

  - 静音： 搞了点声音玩，不用可勾选。

  - 自动更新：自动更新游戏

  - 维护等待： 限制脚本自动更新游戏时间避免 17/SQ 未完成更新发布！

  - 同时进游账号：选角界面点击"进入江湖"比较耗 cpu，此为限制同时进入江湖加载的账号数。

  - 开线尝试次数：开了线尝试次数 N 限制，若选多条线如 3-4-5，3、4 线同时开线 3 线尝试 N 次后都失败若 4 线还开着会继续尝试 4 线以此类推；越靠前的线越先尝试，如选择 5-4-3，3 条线同时开会先尝试 5线。

    > 脚本做了卡线流程，若卡线会 5 分钟后进 1 线尝试！

- 基址
  供观察复制

- 子线程

  - 子线程N秒循环：内置最小都给了个 2s 循环省 cpu。
  - 定时任务：可关机、关所有游戏 2m+ 后重登、关所有游戏且停止登录。主要用于之前知道什么时候掉线定时任务，但我把路由器改桥接了不掉线了没用过了，自测自用。

- test

  - 老泫勃派任务：需打开NPC后！！！ 选择历练/糖果任务提交 N 次。	
  - 右击：选择坐标后右击使用物品 N 次。(之前挂不能自动使用物品写着玩的)
  - 手开游戏：测试用直接 shell 打开表格第一个号大区对应的 Client.exe 玩玩避免手开 launcher.exe。

### 查线

查线有俩种方式。

- 打开 查线器.exe 查字每条线尾部 "%"若有即开线。

  > ps:曾开了几天 CPU 飙升！ 且一直挂在最前影响做其他事儿。弃了！

- 现使用 VC6 写的按键精灵插件 [searchLines/RxSocket.dll](searchLines/RXSocket.dll) 。包含如下方法：

  - getPartServerLines 获取分区服务器的线路状况(字符串分区,字符串服务器(末尾加\"(新服)\"))[0满,1未满,逗号分隔]
  - getByteArray 获取分区返回数据(字符串分区)[字节数组]
  - writeData2Clickable 修改禁用状态(长整型窗口句柄,字符串内存地址('|'分隔的十六进制字符串地址))[0异常,1成功]
  - writeData 修改内存数据(长整型窗口句柄,字符串内存地址,字符串二进制数据(小端))[0异常,1成功]
  - getModuleSize 获取模块大小(长整型窗口句柄,字符串模块名称)[0异常]

## 注意

- **首次使用脚本，若有账号登录着，需添加该账号后重启脚本才能识别到！否则可能重复登录！**

- 脚本存储密码使用自带的加解密，可自行修改密钥 cipher 随便赋值。(最大 16 个字符！)

- 白嫖账号限制 30 个应该够了，若改需同时修改 useDir、cpdFile 。

- 游戏分辨率 1280*720 (若不同影响鼠标点击，可自行修改 login2()、login3() 的点击位置)。

- 已屏蔽挂控制定点，脚本实时控制保存定点与攻击距离。
  - 相应的账号操作里的上线后的选项里带""定"的可选但没用！
  - 上线后的选项里的"开店"没写因为没用过，要用自写下。
  
- 账号角色名必须，很多都是以此为判断依据。

- ""开查线器"是用的电五沉香，自改。

- 掉线分为本地掉线与服务器掉线。本地可检测，服务器需配合挂界面 保护-勾选掉线-退出游戏？

  > 服务器掉线没搞懂挂是咋判断的都没有下拉框选项的判断条件，测试用了下不选"退出游戏"也直接关掉游戏了，可能是超时啥的？待研究暂时配合使用？

- 其他没想到的可能还有，注释写得蛮详细的，自查自改自勉！......

## 最后

随便玩玩，不保证完美运行，可能有 bug，有空随缘更新，不保证！可能都要弃了觉得无聊没意思了！
