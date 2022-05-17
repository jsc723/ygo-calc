# 游戏王抽卡模拟器
## 使用方法
### 1. 创建一个yml文件，里面格式如下
```yaml

deck：配置牌组
  size: 牌组卡牌总数
  cards：定义卡片
       卡牌名: 每张卡名字任意
            count: 重复次数（默认一张），-1代表根据牌组卡牌总数自动计算（只能有一个-1），
            attribute: 一个列表，定义赋予该卡的属性（标签）
                       属性内容自定义，例如怪兽卡、魔法卡，本家卡、泛用卡、手坑等等，均用字符串表示
       注意卡牌总数不要超过之前定义的size
  classes: 定义卡片的类型，每个class是一个类型，格式为
      类型名: 条件1 条件2 条件3 ...
            每个条件之间是逻辑与（&&）关系
            每个条件的格式必须是以下一种：
                1. 牌组（deck）中定义的卡牌名
                2. 定义在当前class之前的类型名 （例如class1已经定义，当前在定义class2，则定义class2时可以引用class1）
                3. a:属性  ->  要求该类型包含某种特定的属性
                4. A:属性（模糊匹配）  -> 要求该类型包含某种特定的属性，后面的表达式可以带*（匹配任意个任意字符）或？（匹配一个任意字符）
                5. !条件   ->  逻辑取反（要求后面跟的条件为假）
simulate: 模拟器配置
    count：模拟次数
    start_card: 初始手牌为多少张卡（默认5）
    turns: 模拟多少回合 （默认5）
    tests：列举想要关注的主题（例：展开、阻抗），每个主题之间互不相干
        主题名: 列举当前主题希望得到的手牌组合，每个组合之间是逻辑与（||）关系
            组合名: 定义一种希望满足的条件，列举需要的手牌
                - 手牌类型1 （引用deck定义的卡牌名或classes定义的类型名）
                ... 
                （注：当一张卡牌匹配了一种手牌类型之后，这张卡牌不会被用于匹配其他手牌类型，列举在前的手牌类型具有更高的优先级）
```
注：文档里请只写半角字符（不要写中文不然可能出错）
#### 例
```yaml

deck:
    size: 40
    cards:
        w-red:
            count: 3
            attribute: ['M', 'low', 'witches', 'start'] 
        w-yellow:
            count: 2
            attribute: ['M', 'low', 'witches', 'start']
        w-grey:
            count: 2
            attribute: ['M', 'low', 'witches', 'start']
        w-green:
            attribute: ['M', 'low', 'witches', 'start']
        w-blue:
            count: 2
            attribute: ['M', 'witches']
        w-black:
            count: 2
            attribute: ['M', 'witches']
        w-white:
            attribute: ['M', 'witches']
        w-creation:
            count: 3
            attribute: ['H1', 'witches', 'start']
        w-sabo:
            count: 2
            attribute: ['H1', 'witches']
        w-colab:
            attribute: ['H1', 'witches']
        w-demo:
            count: 2
            attribute: ['H2', 'witches']
        w-wind:
            attribute: ['H2', 'witches']
        w-street:
            attribute: ['H0', 'witches']
        w-scroll:
            attribute: ['H0', 'witches']
        sato:
            count: 4
            attribute: ['Hf']
        dhero-fusion:
            count: 2
            attribute: ['H1', 'dhero']
        w-patoro:
            attribute: ['W', 'witches']
        urara:
            count: 2
            attribute: ['M', 'handtrap']
        huai-shou:
            count: 1
            attribute: ['M', 'huai-shou']
        others:
            count: -1
    classes:
        Witches: A:w*ch?s
        Witches-Monster: Witches  a:M
        Witches-Low-Monster: Witches  a:M  a:low
        Witches-High-Monster: Witches-Monster  !a:low
        Witches-Magic: Witches  A:H?
        Witches-Search: w-creation
        Witches-Start: a:start
        Field: sato
        DHERO-Fusion: dhero-fusion
        Hand-Trap: a:handtrap
simulate:
    count: 50000
    start_card: 5
    turns: 6
    tests:
        Expand:
            witches1:
                - Witches-Start
                - Witches-Magic
            witches2:
                - Witches-High-Monster
                - w-demo
                - Witches-Magic
            dhero:
                - DHERO-Fusion
        Field:
            field:
                - Field
        Resist:
            hand-trap:
                - Hand-Trap
            huai-shou:
                - huai-shou
    
```
在`deck`中，我定义了一个40卡魔女术牌组，从上至下依次是魔女术本家的7种怪，7种本家魔法卡，魔法族之村（sato），
融合命运，灰流，坏兽，其他卡

我把所有怪兽都赋予`M`标签，所有魔法卡按照类型分别赋予`H1` `H2` `H0` `Hf`（通常，速攻，永继，场地）
魔女术本家卡片赋予`witches`标签

有初动能力的本家卡赋予`start`标签，魔女术下级怪赋予`low`标签

在`classes`之中，我定义了一些类型，这里举两个例子：
```
Witches: A:w*ch?s 
```
`w*ch?s`模糊匹配任意以w开头，之后跟任意字符串，然后跟ch，然后跟一个任意字符，结尾是s的属性
在这个牌组中，能被匹配到的属性只有`witches`，所以这条这里也可以写`a:witches`，只不过我为了演示模糊匹配特意这么写

```
Witches-High-Monster: Witches-Monster  !a:low
```
这个类型定义了魔女术上级怪，它需要同时满足两个条件：

1. 是魔女术怪兽（`Witches-Monster`）
2. 不是下级怪（`!a:low`）

这里的Witches-Monster必须定义在Witches-High-Monster之前才能被引用

然后，在`tests`中，我有三个关心的主题，分别是展开，场地卡，阻抗

其中展开又分为本家的两种展开和凤凰人展开，场地卡只有魔法族之村，阻抗又分成手坑和坏兽（暂且归为阻抗）每一中主题里，希望得到什么样的组合，就直接列举出来：

本家第一种展开（witches1），我希望手上有一张本家初动（Witches-Start：下级怪或检索魔法卡），并且还有另一张任意本家魔法卡（Witches-Magic），其他手牌任意

本家第二种展开（witches2），我希望手上有一张本家高级怪，一张演示（出高级怪），和另一张任意本家魔法卡

第三种展开（dhero），只要我手上有融合命运就行（DHERO-Fusion）

程序的输出中，对于每一个主题，每一个回合，都会显示每个主题成功的概率，以及每种组合单独的概率
（输出样例在下面）

### 2. 运行程序
假设在当前目录下有
```
example.yml
ygo-calc.exe
```
则可以通过如下任意方式运行程序：
1. 双击ygo-calc.exe，在弹出的窗口中输入example.exe并按回车
2. 把example.yml的图标拖拽至ygo-calc.exe上直接运行
3. 打开控制台（cmd或powershell），cd至当前目录，输入`./ygo-calc.exe example.yml`

## 程序输出样例
```
$ ./ygo-calc.exe example.yml
size: 40
cards:
name: w-red, count: 3, attribute: [M low start witches ]
name: w-yellow, count: 2, attribute: [M low start witches ]
name: w-grey, count: 2, attribute: [M low start witches ]
name: w-green, count: 1, attribute: [M low start witches ]
name: w-blue, count: 2, attribute: [M witches ]
name: w-black, count: 2, attribute: [M witches ]
name: w-white, count: 1, attribute: [M witches ]
name: w-creation, count: 3, attribute: [H1 start witches ]
name: w-sabo, count: 2, attribute: [H1 witches ]
name: w-colab, count: 1, attribute: [H1 witches ]
name: w-demo, count: 2, attribute: [H2 witches ]
name: w-wind, count: 1, attribute: [H2 witches ]
name: w-street, count: 1, attribute: [H0 witches ]
name: w-scroll, count: 1, attribute: [H0 witches ]
name: sato, count: 4, attribute: [Hf ]
name: dhero-fusion, count: 2, attribute: [H1 dhero ]
name: urara, count: 2, attribute: [M handtrap ]
name: huai-shou, count: 1, attribute: [M huai-shou ]
name: others, count: 7

w-creation: name==w-creation
Hand-Trap: attr=handtrap
Witches-High-Monster: attr~w*ch?s && attr=M && !attr=low
w-red: name==w-red
w-green: name==w-green
w-black: name==w-black
w-yellow: name==w-yellow
w-demo: name==w-demo
w-white: name==w-white
w-grey: name==w-grey
w-blue: name==w-blue
w-sabo: name==w-sabo
w-colab: name==w-colab
w-wind: name==w-wind
w-street: name==w-street
w-scroll: name==w-scroll
sato: name==sato
DHERO-Fusion: name==dhero-fusion
dhero-fusion: name==dhero-fusion
urara: name==urara
huai-shou: name==huai-shou
others: name==others
Witches: attr~w*ch?s
Witches-Monster: attr~w*ch?s && attr=M
Witches-Low-Monster: attr~w*ch?s && attr=M && attr=low
Witches-Magic: attr~w*ch?s && attr~H?
Witches-Search: name==w-creation
Witches-Start: attr=start
Field: name==sato

Expand: witches1: Witches-Start  Witches-Magic
witches2: Witches-High-Monster  w-demo  Witches-Magic
dhero: DHERO-Fusion

Field: field: Field

Resist: hand-trap: Hand-Trap
huai-shou: huai-shou


Simulate 50000 times...
Time used: 2329ms
Topic: Expand
Turn 1 total: 74.25%  witches1: 60.59%  witches2: 6.49%  dhero: 23.54%
Turn 2 total: 83.54%  witches1: 71.60%  witches2: 10.81%  dhero: 27.88%
Turn 3 total: 90.04%  witches1: 80.17%  witches2: 15.69%  dhero: 32.15%
Turn 4 total: 93.95%  witches1: 86.49%  witches2: 20.84%  dhero: 36.16%
Turn 5 total: 96.51%  witches1: 91.03%  witches2: 26.15%  dhero: 40.15%
Turn 6 total: 98.03%  witches1: 94.10%  witches2: 31.48%  dhero: 44.03%

Topic: Field
Turn 1 total: 42.98%  field: 42.98%
Turn 2 total: 49.54%  field: 49.54%
Turn 3 total: 55.28%  field: 55.28%
Turn 4 total: 60.84%  field: 60.84%
Turn 5 total: 65.68%  field: 65.68%
Turn 6 total: 70.06%  field: 70.06%

Topic: Resist
Turn 1 total: 33.83%  hand-trap: 23.63%  huai-shou: 12.56%
Turn 2 total: 39.47%  hand-trap: 27.99%  huai-shou: 15.06%
Turn 3 total: 44.80%  hand-trap: 32.22%  huai-shou: 17.52%
Turn 4 total: 49.58%  hand-trap: 36.13%  huai-shou: 19.94%
Turn 5 total: 54.30%  hand-trap: 40.11%  huai-shou: 22.43%
Turn 6 total: 58.76%  hand-trap: 44.03%  huai-shou: 24.94%

```

## 常见问题
1. 为什么定义类型（class）的时候只支持逻辑与，不支持逻辑或？

- 因为标签和模糊匹配已经间接实现了逻辑或，选取牌组的任何子集都可以用标签解决

2. 为什么组合那里不支持表达式（例如：A类牌+B类牌>1 && C类牌==0）？

- 因为直接把组合列举出来更直观，而且也没有必要。例如上面这个例子，可以定义

- D类牌= A类牌或B类牌 （给所有A类牌和B类牌标注一个相同的标签）

- E类牌=！C类牌
  

如有意向参与开发欢迎提交PR
  
         

