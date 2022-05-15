# 游戏王抽卡模拟器
## 使用方法
### 1. 创建一个yml文件，里面格式如下
```yaml
deck:
  size: 40
  cards:
    card1:
      count: 3
      attribute: ['attr1', 'attr2']
    card2:
      count: 2
      attribute: ['attr2', 'attr3', 'attr4']
    others:
      count: -1
  classes:
    class1: a:attr1
    class2: card2
    class3: a:attr2 !a:attr3
simulate:
  count: 50000
  start_card: 5
  turns: 6
  separate: no
  tests:
    test1:
      - class1
      - class2
    test2:
      - class2
      - class3
```
```
deck：配置牌组
  size: 牌组卡牌总数
  cards：定义卡片，每张卡名字任意, count代表重复次数（默认一张），-1代表根据牌组卡牌总数自动计算（只能有一个-1），attribute是赋予该卡的属性（默认空）
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
simulate: 配置模拟
    count：模拟次数
    start_card: 初始手牌为多少张卡（默认5）
    turns: 模拟多少回合 （默认5）
    separate：如果为yes，则还会计算分别每个test成功的概率为多少
    tests：定义希望手牌满足的条件（例如能展开的组合），每个条件是逻辑与关系
        test1（名字任意）: 定义一种希望满足的条件，列举需要的手牌
            - 手牌类型1 （引用卡牌名（deck中定义）或卡牌类型名（classes中定义））
            - 手牌类型2
            ... 
            （注：当一张卡牌匹配了一种手牌类型之后，这张卡牌不会被用于匹配其他手牌类型，列举在前的手牌类型具有更高的优先级）
```
具体的例子可以参照`test/example.yml`

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
name: w-patoro, count: 1, attribute: [W witches ]
name: others, count: 9

w-creation: name==w-creation
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
w-patoro: name==w-patoro
others: name==others
Witches: attr~w*ch?s
Witches-Monster: attr~w*ch?s && attr=M
Witches-Low-Monster: attr~w*ch?s && attr=M && attr=low
Witches-Magic: attr~w*ch?s && attr~H?
Witches-Search: name==w-creation
Witches-Start: attr=start
Field: name==sato

combo1: Witches-Start  Witches-Magic
combo2: Witches-High-Monster  w-demo  Witches-Magic
combo3: DHERO-Fusion

Simulate 50000 times...
Time used: 1463ms
Turn 1 total: 74.61%  combo1: 61.09%  combo2: 6.58%  combo3: 23.51%
Turn 2 total: 83.99%  combo1: 72.22%  combo2: 10.77%  combo3: 27.83%
Turn 3 total: 90.14%  combo1: 80.65%  combo2: 15.33%  combo3: 32.18%
Turn 4 total: 94.04%  combo1: 86.73%  combo2: 20.54%  combo3: 36.15%
Turn 5 total: 96.54%  combo1: 91.20%  combo2: 25.88%  combo3: 40.11%
Turn 6 total: 98.01%  combo1: 94.23%  combo2: 31.19%  combo3: 43.97%
```
  
  
         

