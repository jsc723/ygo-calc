

# 游戏王概率计算器ygo-calc v2

## 介绍

你是否担心自己的卡组有以下问题：

- 先手卡手动不了
- 堆墓5张什么都没触发
- 后手抽不到手坑和解场卡，失去玩游戏王的权利

如果是，那这个工具可以帮你模拟计算以事件发生的概率。

大致原理是：

- 用户定义卡组（不包括额外）
- 用户列出所有希望抽到的手牌的组合（或者堆墓希望堆到的牌的组合）
- 工具模拟抽卡/堆墓，统计抽到希望的组合的概率

## 快速入门
### 1. 创建YAML文件

创建一个格式如下的YAML文件`example.yml`:

```yaml
deck:
    cards:
        kowakuma-trion:
            count: 3
            attribute: ['monstor', 'kowakuma']
            description: '特里恩虫惑魔'
        kowakuma-tio:
            count: 3
            attribute: ['monstor', 'kowakuma']
            description: '蒂奥虫惑魔'
        kowakuma-ranka:
            count: 3
            attribute: ['monstor', 'kowakuma']
            description: '兰卡虫惑魔'
        kowakuma-jiner:
            count: 3
            attribute: ['monstor', 'kowakuma']
            description: '吉娜虫惑魔'
        kowakuma-atora:
            attribute: ['monstor', 'kowakuma']
            description: '阿特拉虫惑魔'
        urara:
            count: 3
            attribute: ['monstor', 'hand-trap']
            description: '灰流丽'
        zou-g:
            count: 3
            attribute: ['monstor', 'hand-trap']
            description: '增殖G'
        bu-mian-jia:
            count: 3
            attribute: ['trap']
            description: '布面甲'
        houyou:
            count: 3
            attribute: ['trap', 'hand-trap']
            description: '泡影'
        other-trap:
            count: 15
            attribute: ['trap']
            description: '红坑'
        
    alias:
        HandTrap: A:hand*                   #匹配灰流丽，增殖G，泡影
        MonstorHandTrap: HandTrap a:monstor #只匹配灰流丽，增殖G
        
simulate:
    count: 1000 #模拟的次数
    confidence-interval: true #显示95%置信区间，默认false
    tests: #列举想要关注的主题（例：展开、阻抗），每个主题之间互不相干
        test-expend:  #列举当前主题希望得到的手牌/墓地组合，主题名称任意，有任意一个组合满足则任务当前主题满足
            combos:   
                A1:   #每种组合的名称任意
                    score: 2 #可以为每个组合设定一个分数(score)，比如能做出多少个阻抗
                    hand:      #列出希望抽到的手牌
                        - a:kowakuma
                        - a:trap
                A2:
                    score: 4
                    hand:
                        - a:kowakuma
                        - bu-mian-jia
                        - a:trap
        test-hand-trap:
            start-card: 5  #设置起始手牌数，默认5
            combos:
                HT1:
                    score: '|H.a:hand-trap|'   #计算手上手坑的个数，详见进阶篇
                    hand:
                        - HandTrap
                HT2:
                    score: '|H.a:hand-trap|'
                    hand:
                        - HandTrap
                        - HandTrap
                urara-2:
                    condition: '(== |H.urara| 2)' #也可以编程使用表达式，计算两只灰流同时上手的概率
```
以上yaml文件先在`deck`中定义了一个虫惑魔卡组（md版本），`deck.cards` 下面列出每种卡的
- 数量：`count`，如果不填默认1
- 属性：`attribute`， 一个字符串列表，属性的内容任意，也可以不填
- 描述：`description`，比如中文名，也可以不填

每种`combos`的`score`必须是一个整数或者`<number>`表达式（见进阶篇），如果不填则默认为1

在`deck.alias`中可以给一些条件的组合起别名，格式如下

```
    alias:
        <别名>: <条件列表>
            <条件列表>的格式为<condition1> <condition2>...，中间用空格隔开
            每个condition之间是逻辑与（&&）关系
            每个condition的格式必须是以下一种：
                1. 牌组（deck）中定义的卡牌名
                2. a:属性  ->  要求该类型包含某种特定的属性
                3. A:属性（模糊匹配）  -> 要求该类型包含某种特定的属性，后面的表达式可以带*（匹配任意个任意字符）或？（匹配一个任意字符）
                4. !<condition>   ->  逻辑取反（要求后面跟的条件为假）
                5. 定义在此项之前的别名
```

在`simulate`中定义需要模拟抽卡的次数(`count`), 在`tests`中列出想要关注的主题， 格式如下

```
    tests:
        <主题名>: 
            <组合名>: 
                score: <分数> #默认1，topic的分数为所有匹配的组合的最高分
                condition: <number> #如果<number>表达式的值不为0则匹配，否则不匹配，score返回为0
                hand:        #列出想抽到的手牌组合
                    - condition1 （卡名，alias定义的别名，或者<条件列表>）
                    - condition2 
                    ... 
                grave:       #列出墓地里想堆到的牌（详见进阶篇）
                	- condition3
                	...
```
每种组合中`condition`,`hand`,`grave`可以选填任意一个，两个，或者都填

当一张卡牌匹配了一种手牌类型之后，这张卡牌不会被用于匹配其他手牌类型，列举在前的手牌类型具有更高的优先级

注：文档里除了`description`请只写半角字符

### 2. 运行程序

假设在当前目录下有

```
example.yml
ygo-calc.exe
```

则可以通过如下任意方式运行程序：

1. 双击ygo-calc.exe，在弹出的窗口中输入example.yml并按回车
2. 把example.yml的图标拖拽至ygo-calc.exe上直接运行
3. 打开控制台（cmd或powershell），cd至当前目录，输入`./ygo-calc.exe example.yml`

### 3. 输出样例

```
Simulate 1000 times...
Time used: 97ms
Topic: test-expend
First turn average success rate: 85.50% +- 2.18%  average score: 2.63 +- 0.11
    A1: 85.50% +- 2.18%      A2: 25.00% +- 2.68%

Topic: test-hand-trap
First turn average success rate: 74.40% +- 2.70%  average score: 1.12 +- 0.05
    HT1: 74.40% +- 2.70%      HT2: 30.70% +- 2.86%      urara-2: 3.60% +- 1.15%

```
注：如果中文description不能正常显示可能是控制台code page的问题，试试控制台中输入`chcp 65001`

## 进阶篇
以上的功能能满足大多数的卡组，可是不好计算有滤抽功能的卡（比如各种壶，万宝槌，手牌抹杀等）时的概率，也不能模拟卡的效果堆墓。进阶篇中将解决这个问题。大致原理是：
- 定义卡片的同时编写脚本告诉模拟器这张卡的效果（比如可以抽卡，可以堆墓，可以检索）
- 模拟器在抽到牌后自动把手牌、墓地中所有能发动的卡牌都发动，直到没有卡牌能发动为止
- 模拟器把手牌、墓地中的卡与列出的希望得到的组合做匹配
### 例
以下文件定义了一个珠泪卡组，并且定义了先手每张卡如何用于展开（可能不全，也不完全和真实效果匹配，只是一个实例）
```yaml
deck:
    cards:
        zhulei-renyu-2:
            count: 2
            attribute: ['M', 'zhulei', 'renyu']
            description: '2星人鱼'
            program: #一个字符串列表，每行代表一个效果，一张卡可以有多个效果
              - '[1]/(> summon 0);(= summon (- summon 1));@;(# X F);(# D.3 B);(if (and (> |H.xian-sheng| 0) (< 0 |H.xian-sheng|)) (# D.3 B) ())'
        zhulei-renyu-3:
            count: 2
            attribute: ['M', 'zhulei', 'renyu']
            description: '3星人鱼'
        zhulei-renyu-4:
            count: 2
            attribute: ['M', 'zhulei', 'renyu']
            description: '4星人鱼'
            program: 
              - '[1]/(> |H.M| 1);@;(# X F);(if (> |H.a:renyu| 0) (# H.a:renyu.1 B) (# H.M B));(# D.3 B);(if (> |H.xian-sheng| 0) (# D.3 B) ())'
        zhulei-nanren:
            count: 3
            attribute: ['M', 'zhulei', 'nanren']
            description: '珠泪男人'
            program: 
              - '[1]/(== summon 1);(= summon (- summon 1));@;(# X F);(# D.a:renyu.1 B);(if (> |H.xian-sheng| 0) (# D.3 B) ())'
        gu-jian-bin:
            count: 2
            attribute: ['M', 'di-tian-shi', 'bin']
            description: '古尖兵'
            program: 
              - '[1B]@;(# D.5 B)'
        gu-wei-bin:
            count: 2
            attribute: ['M', 'di-tian-shi', 'bin']
            description: '古卫兵'
            program: 
              - '[1B]@;(# D.5 B)'
        jian-shen-guan:
            count: 2
            attribute: ['M', 'di-tian-shi']
            description: '剑神官'
            program: 
              - '[1]/(> |H.a:bin| 1);@;(# X F);(# H.a:bin.1 B)'
        su-shen-xiang:
            count: 2
            attribute: ['M', 'di-tian-shi']
            description: '宿神像'
            program: 
              - '[1]/(> |H.a:bin| 1);@;(# X F);(# H.a:bin.1 B)'
        shen-wu:
            count: 3
            attribute: ['M']
            description: '神巫'
            program: 
              - '[1]/(> summon 0);(= summon (- summon 1));@;(# X F);(# D.a:bin.1 B);(if (> |H.xian-sheng| 0) (# D.3 B) ())'
        zhenzhujie:
            count: 2
            attribute: ['mahou']
            description: '珠泪场地'
            program: 
              - '[1]@;(# X F);(if (> |D.zhulei-nanren| 0) (# D.zhulei-nanren.1 H) ())'
        ronghe:
            count: 1
            attribute: ['mahou']
            description: '融合'
        zenyuan:
            count: 1
            attribute: ['mahou']
            description: '增援'
            program: 
              - '/(> |D.zhulei-nanren| 0);@;(# X B);(# D.zhulei-nanren.1 H))'
        maizang:
            count: 1
            attribute: ['mahou']
            description: '埋葬'
            program: 
              - '@;(# X B);(# D.a:renyu.1 B))'
        mo-shen-wang:
            count: 1
            attribute: ['M']
            description: '魔神王'
            program: 
              - '[1]/(> |D.ronghe| 0);@;(# X B);(# D.ronghe.1 H))'
        xian-sheng:
            count: 3
            attribute: ['mahou']
            description: '弦声'
        can-xiang:
            count: 3
            attribute: ['trap']
            description: '残响'
        hand-trap:
            count: 8
            description: '手坑'
simulate:
    debug: true #要求模拟器显示每次抽到牌后的详细执行步骤
    count: 10
    tests:
        test-expend:
            exec-program: true #要求模拟器自动发动卡牌（默认false)
            header: '(= summon 1)' #回合开始时先执行header，一般用于定义全局变量
            combos:
                A1:
                    grave:         
                        - a:renyu  #墓地堆到人鱼就算能成功展开
                A2:
                    hand:
                        - ronghe    #手上有融合也算能成功展开
                        - a:zhulei
        test-hand-trap:
            start-card: 5
            combos:
                H1:
                    hand:
                        - hand-trap
                H2:
                    score: '(+ |D.3.a:renyu| |D.3.a:bin|)'
                    hand:
                        - zhulei-renyu-3

```
注：目前还不支持诱发效果，所以地天使被堆墓时的诱发效果全都写成了主动效果

### 输出样例
```
$ ./ygo-calc.exe zhulei.yml
(前略)
-----------run-------------
execStatement(@)
execStatement((# X F))
move {zhenzhujie(珠泪场地), }from H to F
execStatement((if (> |D.zhulei-nanren| 0) (# D.zhulei-nanren.1 H) ()))
move {zhulei-nanren(珠泪男人), }from D to H
executed card zhenzhujie[0] [珠泪场地]
execStatement(/(> summon 0))
execStatement((= summon (- summon 1)))
execStatement(@)
execStatement((# X F))
move {zhulei-renyu-2(2星人鱼), }from H to F
execStatement((# D.3 B))
move {hand-trap(手坑), zenyuan(增援), zhulei-renyu-2(2星人鱼), }from D to B
execStatement((if (and (> |H.xian-sheng| 0) (< 0 |H.xian-sheng|)) (# D.3 B) ()))
executed card zhulei-renyu-2[0] [2星人鱼]
execStatement(/(> |H.M| 1))
execStatement(/(== summon 1))
execStatement((= summon 1))
-----------run-------------
execStatement(/(> summon 0))
execStatement((= summon (- summon 1)))
execStatement(@)
execStatement((# X F))
move {zhulei-renyu-2(2星人鱼), }from H to F
execStatement((# D.3 B))
move {hand-trap(手坑), shen-wu(神巫), zhenzhujie(珠泪场地), }from D to B
execStatement((if (and (> |H.xian-sheng| 0) (< 0 |H.xian-sheng|)) (# D.3 B) ()))
executed card zhulei-renyu-2[0] [2星人鱼]
execStatement(/(== summon 1))
execStatement(/(> |H.a:bin| 1))
Time used: 21ms
Topic: test-expend
First turn average success rate: 80.00%    average score: 0.80
    A1: 80.00%      A2: 10.00%

Topic: test-hand-trap
First turn average success rate: 70.00%    average score: 0.70
    H1: 50.00%      H2: 30.00%
```
可以看到模拟器会自动发动卡牌展开（只需要展开到随机部分结束就行了）

### 卡牌效果脚本
每个效果的语法详细定义、用法如下（注：//中间的部分是正则表达式）
```
<effect> -> [<effect-attributes>]<statements>
       | -> <statements>
<effect-attributes> -> {1^HB}的任意排列组合
    "1代表卡名一回合一次，^代表只能在回合开始时发动"
    "H代表在手牌时可以发动，B代表在墓地时可以发动，如果不写H或B则默认是在手牌发动"

<statements> ->  <statement>
             |-> <statements>;<statement>            "语句之间用;隔开"
<statement> ->  @                                    "发动效果，见注意事项"
            |-> /<number>                            "如果<number>的值为0则不再执行后续语句"
            |-> <expression>
<expression> -> () 
                "空表达式，什么都不做"
            |-> (% <number>)
                "从卡组抽<number>张"   
            |-> (# <cardset> <card-collection>) 
                "把<cardset>的卡全部加入到<card-collection>中"   
            |-> ($ <cardset> <number> <cardset>) 
                "从第一个<cardset>选择<number>张加入第二个<cardset>"
            |-> (! <string>)
                "禁止本回合再执行<string>命令，例如(! %)禁止抽卡"
            |-> (= <varname> <number>)
                "把变量<varname>的值设置为<number>"
                "如果<varname>的长度在两个字符以上，则这个变量的值在当前回合一直有效（全局变量）"
                "否则只在当前效果执行期间有效（临时变量）"
            |-> (if <number> <expression> <expression>)
                "如果<number>不为0则执行第一个<expression>，否则执行第二个"
<varname> -> [a-z]+
<string> -> /[^\s()]/
<number-literal> -> /[0-9]+/
<number> -> <number-literal>
        |-> <var-name>
        |-> |<cardset>|
            "得到<cardset>的大小(卡的数量)"
        |-> (<op> <number> <number>)
            "相当于<第一个number> <op> <第二个number>"
<op> -> +|-|>|<|==|and|or|r
    "其中r代表random，(r x y) 代表随机取一个[x, y]之间的整数"
<card-collection> -> H|D|F|B|J|X
    "H: 手牌，D：卡组，F：场上，B：墓地，J：除外，X：正在执行的这张卡"
<cardset> -> <card-collection>
        |->  <card-collection><cardset-filters>
    "<cardset>从一个<card-collection>开始，后面可以跟一连串的<cardset-filter>"
    "每个<cardset-filter>用.隔开"
<cardset-filters> -> <cardset-filter>
                |->  <cardset-filter><cardset-filters>
<cardset-filter> -> .<card-name>
                    "选取集合中所有卡名为<card-name>的卡"
                |-> .a:<card-attribute>
                    "选取集合中所有拥有<card-attribute>属性的卡（不支持模糊匹配）"
                |-> .<number-literal>
                    "选取集合中前<number-literal>张卡"
    
```
#### 注意事项
- **每个效果必须有一句`@`语句表示效果成功发动**，在效果发动之前尽量不要移动任何卡（否则可能出bug，也可能没bug）
#### 例
```
'[1]/(> summon 0);(= summon (- summon 1));@;(# X F);(# D.a:bin.1 B);(if (> |H.xian-sheng| 0) (# D.3 B) ())'
```
这个效果的意思如下:
- `[1]`：卡名一回合一次
- `/(> summon 0)`：当summon变量的值大于0时（自己这回合还没召唤过怪兽时）
- `(= summon (- summon 1))`: summon的值减少1
- `@`：发动这张卡
- `(# X F)`：把这张卡（`X`）移动到场上（`F`）（召唤）
- `(# D.a:bin.1 B)`：从牌组（`D`）选一只具有`bin`属性的卡（即古尖兵或古卫兵）送入墓地(`B`)
- `(if (> |H.xian-sheng| 0) (# D.3 B) ())` 如果手牌中有弦声`(> |H.xian-sheng| 0)`, 则把牌组顶端3张卡(`D.3`)送入墓地(`B`)，否则什么也不做(`()`) (注：严格来说场上有弦声才能触发堆3张的效果，不过这里为了简化程序就写了手牌)



