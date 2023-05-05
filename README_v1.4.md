# 游戏王概率计算器ygo-calc v1.4
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
  conditions: 给条件或条件的组合起别名，格式为
      条件名: <条件列表>
            <条件列表>的格式为`<condition1> <condition2>...`，中间用空格隔开
            每个condition之间是逻辑与（&&）关系
            每个condition的格式必须是以下一种：
                1. 牌组（deck）中定义的卡牌名
                2. a:属性  ->  要求该类型包含某种特定的属性
                3. A:属性（模糊匹配）  -> 要求该类型包含某种特定的属性，后面的表达式可以带*（匹配任意个任意字符）或？（匹配一个任意字符）
                4. !条件   ->  逻辑取反（要求后面跟的条件为假）
                5. 定义在此项之前的条件名
simulate: 模拟器配置
    count：模拟次数
    start_card: 初始手牌为多少张卡（默认5）
    turns: 模拟多少回合 （默认5）
    tests：列举想要关注的主题（例：展开、阻抗），每个主题之间互不相干
        主题名: 列举当前主题希望得到的手牌组合，每个组合之间是逻辑与（||）关系
            组合名: 定义一种希望满足的条件，列举需要的手牌
                score: 该组合的分数（比如有多少个阻抗，默认1.0）
                conditions:
                    - condition1 （conditions定义的条件名，或者<条件列表>）
                    - condition2 
                    ... 
                    （注：当一张卡牌匹配了一种手牌类型之后，这张卡牌不会被用于匹配其他手牌类型，列举在前的手牌类型具有更高的优先级）
```
注：文档里请只写半角字符（不要写中文不然可能出错）
#### 例
```yaml

deck:
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
        urara:
            count: 2
            attribute: ['M', 'handtrap']
        huai-shou:
            count: 1
            attribute: ['M', 'huai-shou']
        others:
            count: -1
    conditions:
        Witches: A:w*ch?s
        Witches-Monster: Witches  a:M
        Witches-Low-Monster: Witches  a:M  a:low
        Witches-High-Monster: Witches-Monster  !a:low
        Witches-Magic: Witches  A:H?
        Witches-Search: w-creation
simulate:
    count: 1000
    start_card: 5
    turns: 6
    tests:
        Expand:
            witches1:
                score: 1.0
                conditions:
                    - a:start
                    - Witches-Magic
            witches2:
                conditions:
                    - Witches-High-Monster
                    - w-demo
                    - Witches-Magic
            dhero:
                score: 2.0
                conditions:
                    - dhero-fusion
        Field:
            field:
                conditions:
                    - sato
        Resist:
            hand-trap:
                conditions:
                    - a:handtrap
            huai-shou:
                conditions:
                    - huai-shou
    
    
```
在`deck`中，我定义了一个40卡魔女术牌组，从上至下依次是魔女术本家的7种怪，7种本家魔法卡，魔法族之村（sato），
融合命运，灰流丽，坏兽，其他卡

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

本家第一种展开（witches1），我希望手上有一张本家初动（a:start：下级怪或检索魔法卡），并且还有另一张任意本家魔法卡（Witches-Magic），其他手牌任意（这种展开我给的分数是1分）

本家第二种展开（witches2），我希望手上有一张本家高级怪，一张演示（出高级怪），和另一张任意本家魔法卡（这种展开我给的分数也是1分）

第三种展开（dhero），只要我手上有融合命运就行（DHERO-Fusion），这种展开的给的分数是2分

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
Simulate 1000 times...
Time used: 91ms
Topic: Expand
Turn 1 average success rate: 76.00%    average score: 0.99
    witches1: 62.10%      witches2: 6.20%      dhero: 23.10%

Turn 2 average success rate: 84.70%    average score: 1.11
    witches1: 72.20%      witches2: 10.50%      dhero: 26.20%

Turn 3 average success rate: 90.60%    average score: 1.22
    witches1: 80.80%      witches2: 15.40%      dhero: 31.20%

Turn 4 average success rate: 95.00%    average score: 1.29
    witches1: 88.60%      witches2: 20.90%      dhero: 34.50%

Turn 5 average success rate: 97.10%    average score: 1.35
    witches1: 92.10%      witches2: 25.50%      dhero: 38.10%

Turn 6 average success rate: 98.40%    average score: 1.41
    witches1: 94.90%      witches2: 30.70%      dhero: 42.40%


Topic: Field
Turn 1 average success rate: 39.70%    average score: 0.40
    field: 39.70%

Turn 2 average success rate: 45.90%    average score: 0.46
    field: 45.90%

Turn 3 average success rate: 51.90%    average score: 0.52
    field: 51.90%

Turn 4 average success rate: 57.30%    average score: 0.57
    field: 57.30%

Turn 5 average success rate: 62.60%    average score: 0.63
    field: 62.60%

Turn 6 average success rate: 67.50%    average score: 0.68
    field: 67.50%


Topic: Resist
Turn 1 average success rate: 33.70%    average score: 0.34
    hand-trap: 24.20%      huai-shou: 12.30%

Turn 2 average success rate: 38.70%    average score: 0.39
    hand-trap: 27.90%      huai-shou: 14.50%

Turn 3 average success rate: 43.30%    average score: 0.43
    hand-trap: 31.80%      huai-shou: 16.10%

Turn 4 average success rate: 48.60%    average score: 0.49
    hand-trap: 36.10%      huai-shou: 18.40%

Turn 5 average success rate: 53.00%    average score: 0.53
    hand-trap: 39.40%      huai-shou: 21.10%

Turn 6 average success rate: 57.60%    average score: 0.58
    hand-trap: 43.40%      huai-shou: 23.40%


```

## 常见问题
1. 为什么定义条件（condition）的时候只支持逻辑与，不支持逻辑或？

- 因为标签和模糊匹配已经间接实现了逻辑或，选取牌组的任何子集都可以用标签解决

2. 为什么组合那里不支持表达式（例如：A类牌+B类牌>1 && C类牌==0）？

- 因为直接把组合列举出来更直观，而且也没有必要。例如上面这个例子，可以定义

- D类牌= A类牌或B类牌 （给所有A类牌和B类牌标注一个相同的标签）

- E类牌=！C类牌
  

如有意向参与开发欢迎提交PR
  
         

