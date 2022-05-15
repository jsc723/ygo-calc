# 游戏王抽卡模拟器
## 使用方法
1. 创建一个yml文件，里面格式如下
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
  cards：定义卡片，每张卡名字任意, count代表重复次数（默认一张），attribute是赋予该卡的属性（默认空）
         属性内容自定义，例如怪兽卡、魔法卡，本家卡、泛用卡、手坑等等，均用字符串表示
         注意卡牌总数不要超过之前定义的size
  classes: 定义卡片的类型，每个class是一个类型，格式为
      类型名: 条件1 条件2 条件3 ...
      每个条件之间是逻辑与（&&）关系
      每个条件的格式必须是以下一种：
        1. 牌组（deck）中定义的卡牌名
        2. 定义在当前class之前的类型名 （例如class1已经定义，当前在定义class2，则定义class2时可以引用class1）
        3. a:属性  ->  要求该类型包含某种特定的属性
        4. !条件   ->  逻辑取反（要求后面跟的条件为假）
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

## 程序输出样例
```
$ ./ygo-calc.exe example.yml
size: 40
cards:
name: w-red, count: 3, attribute: [M low witches ]
name: w-yellow, count: 2, attribute: [M low witches ]
name: w-grey, count: 2, attribute: [M low witches ]
name: w-green, count: 1, attribute: [M low witches ]
name: w-blue, count: 2, attribute: [M witches ]
name: w-black, count: 2, attribute: [M witches ]
name: w-white, count: 1, attribute: [M witches ]
name: w-creation, count: 3, attribute: [H witches ]
name: w-sabo, count: 2, attribute: [H witches ]
name: w-colab, count: 1, attribute: [H witches ]
name: w-demo, count: 2, attribute: [H witches ]
name: w-wind, count: 1, attribute: [H witches ]
name: w-street, count: 1, attribute: [H witches ]
name: w-scroll, count: 1, attribute: [H witches ]
name: sato, count: 4, attribute: [H ]
name: dhero-fusion, count: 2, attribute: [H dhero ]
name: w-patoro, count: 1, attribute: [W witches ]
name: others, count: 9

w-creation: name==w-creation
Witches-High-Monster: attr=witches && attr=M && !attr=low
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
Witches: attr=witches
Witches-Monster: attr=witches && attr=M
Witches-Low-Monster: attr=witches && attr=M && attr=low
Witches-Magic: attr=witches && attr=H
Witches-Search: name==w-creation
Field: name==sato

combo1: Witches-Low-Monster  Witches-Magic
combo2: Witches-Search  Witches-Magic
combo3: Witches-High-Monster  w-demo  Witches-Magic
combo4: DHERO-Fusion

Simulate 50000 times...
Time used: 977ms
Turn 1 total: 77.15%  combo1: 54.38%  combo2: 23.19%  combo3: 6.75%  combo4: 23.51%
Turn 2 total: 86.08%  combo1: 65.26%  combo2: 30.38%  combo3: 10.92%  combo4: 27.79%
Turn 3 total: 91.81%  combo1: 74.06%  combo2: 37.35%  combo3: 15.73%  combo4: 31.94%
Turn 4 total: 95.22%  combo1: 80.80%  combo2: 43.85%  combo3: 20.92%  combo4: 36.26%
Turn 5 total: 97.24%  combo1: 86.02%  combo2: 49.83%  combo3: 26.23%  combo4: 40.27%
Turn 6 total: 98.40%  combo1: 89.89%  combo2: 55.36%  combo3: 31.54%  combo4: 44.12%
```
  
  
         

