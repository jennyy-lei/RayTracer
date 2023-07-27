mat_ball = gr.material({1,1,1}, {0.45, 0.5, 0.5}, 8, 1, 1.2)
mat_ground = gr.material({160/255, 143/255, 136/255}, {0, 0, 0}, 0)
mat_grass = gr.material({201/255, 255/255, 174/255}, {0.4, 0.4, 0.4}, 100)
mat_trees = gr.material({158/255, 239/255, 136/255}, {0.25, 0.25, 0.25}, 50)
mat_water = gr.material({156/255, 255/255, 253/255}, {0.2, 0.2, 0.2}, 100, 0.3, 1.3)

mat_table = gr.material({0, 0, 0}, {0.5, 0.5, 0.5}, 25)
mat_table:reflection(0.15)
-- mat_wall = gr.material({194/255, 201/255, 185/255}, {0, 0, 0}, 0)
mat_wall = gr.material({225/255, 239/255, 242/255}, {0, 0, 0}, 0)
mat_wall2 = gr.material({242/255, 248/255, 255/255}, {0, 0, 0}, 0)

tex_wood = gr.texture('Assets/Textures/wood3.png')
bump_wall = gr.bump('Assets/Textures/wall5.png')

scene = gr.node('root')

table = gr.cube('table')
table:scale(42, 2, 42)
table:translate(-21, -2, -32)
table:set_material(mat_table)
table:set_texture(tex_wood, 500)
scene:add_child(table)

wall = gr.cube('wall')
wall:scale(38, 38, 0.5)
wall:translate(-19, 3, -19.2)
wall:set_material(mat_wall)
wall:set_bump(bump_wall, 1000)
scene:add_child(wall)

wall2 = gr.cube('wall2')
wall2:scale(42, 42, 1)
wall2:translate(-21, 0, -20.2)
wall2:set_material(mat_wall2)
scene:add_child(wall2)

ball = gr.mesh('ball', 'terrarium/ball.obj')
ball:caustic()
ball:set_material(mat_ball)
ball:translate(0, 0, -1)
scene:add_child(ball)

smallball = gr.mesh('smallball', 'terrarium/smallball.obj')
smallball:caustic()
smallball:scale(0.75, 0.75, 0.75)
smallball:translate(6.4, 0, -1.2)
smallball:set_material(mat_ball)
scene:add_child(smallball)

smallball = gr.mesh('smallball', 'terrarium/smallball.obj')
smallball:caustic()
smallball:scale(1.2, 1.2, 1.2)
smallball:translate(-3.3, 0, 3)
smallball:set_material(mat_ball)
scene:add_child(smallball)

smallball = gr.mesh('smallball', 'terrarium/smallball.obj')
smallball:caustic()
smallball:scale(2.8, 2.8, 2.8)
smallball:translate(-8, 0, -9)
smallball:set_material(mat_ball)
scene:add_child(smallball)

ground = gr.mesh('ground', 'terrarium/ground.obj')
ground:set_material(mat_ground)
ground:translate(0, 0, -1)
scene:add_child(ground)
grass = gr.mesh('grass', 'terrarium/grass.obj')
grass:set_material(mat_grass)
grass:translate(0, 0, -1)
scene:add_child(grass)

water = gr.mesh('water', 'terrarium/water.obj')
water:set_material(mat_water)
water:translate(0, 0, -1)
scene:add_child(water)

for i = 1, 7, 1 do
	trunk = gr.mesh('trunk'..i, 'terrarium/trees/trunk'..i..'.obj')
	leaves = gr.mesh('leaves'..i, 'terrarium/trees/leaves'..i..'.obj')
	trunk:set_material(mat_ground)
	leaves:set_material(mat_trees)
	trunk:translate(0, 0, -1)
	leaves:translate(0, 0, -1)
	scene:add_child(trunk)
	scene:add_child(leaves)
end

white_light = gr.spherical_light({-5, 25, -5}, {0.8, 0.8, 0.8}, {1, 0, 0}, 5)
blue_light = gr.point_light({-30, 30, 30}, {90/255, 120/255, 221/255}, {4, 0, 0})
warm_light = gr.point_light({30, 30, 30}, {255/255, 200/255, 180/255}, {4, 0, 0})

-- gr.render(scene, 'demo/bm.png', 128, 128,
-- 	  {0, 9, 12}, {0, -5, -12}, {0, 1, 0}, 50,
-- 	  {0.37, 0.3, 0.32}, {white_light, blue_light, warm_light})

gr.render(scene, 'demo/5.png', 500, 500,
{0, 9, 12}, {0, -5, -12}, {0, 1, 0}, 50,
{0.37, 0.3, 0.32}, {white_light, blue_light, warm_light})

-- gr.render(scene, 'terrarium-test.png', 128, 128,
-- {0, 9, 12}, {0, -5, -12}, {0, 1, 0}, 50,
-- {0.37, 0.3, 0.32}, {white_light, blue_light, warm_light})
