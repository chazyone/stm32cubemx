这个iwdg

stm32cubemx 配置

prescaler 64 
reload value 625

看门狗时钟 通过查表得到 分频系数 64 最短超时时间就是  40kHz/64 = 0.625Hz
reload-value * preca =超时时间
例如：
625(relvalue) *  64(pre)/40kHz =1s

然后必须1s后调用看门狗刷新，否则会复位



