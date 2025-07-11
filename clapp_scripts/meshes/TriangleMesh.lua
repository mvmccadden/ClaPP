-- Manoel McCadden
-- 05/07/25
-- TriangleMesh.lua

Mesh = {
	vertices = {
		-0.5, -- 0
		-0.5,
		0.0,
		0.5, -- 1
		-0.5,
		0.0,
		0.0, -- 2
		0.5,
		0.0,
	},
	uv = {
		0.5, -- 0
		1.0,
		0.0, -- 1
		0.0,
		1.0, -- 2
		0.0,
	},
	indices = {
		0,
		1,
		2,
	},
	color = {
		0.2,
		0.5,
		0.8,
	},
	type = {
		"TriangleMesh",
	},
}
