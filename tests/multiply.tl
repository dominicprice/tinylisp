(d add
	(q (
		(a b)
		(s a (s 0 b))
	))
)

(d mul
	(q (
		(a b)
		(i b (add a (mul a (s b 1))) 0)
	))
)

(mul 2 2)