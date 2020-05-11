colock prescaler 8
window value  95
downcounter value 127


PCLK1 = 36M

36M/4096/8(pre)=
36M/32768=1098.6328125Hz时钟周期
T=1/1098.6328125 s
127(value)-64=63 个时钟
(63+1)*T=0.05825 s (到调用WWDG中断服务的时间)

window-value>current time>0x40(64) 不在这个范围会复位