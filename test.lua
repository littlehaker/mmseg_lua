require "mmseg"

mmseg.initdict("/home/young/download/lua/mmseg-3.2.14/data");
local words = mmseg.segment("权限不够,不过，这里仍然有一个限制没有解决：不能使用虚成员函数。我爱北京天安门，hello world, 小女孩子们， 通过的和尚未通过的，春光灿烂猪八戒， 他说的确实在理,这事的确定不下来,曹操,法轮功")
for _, v in ipairs(words) do
	print(v)
end
