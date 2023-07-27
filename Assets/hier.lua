-- test for hierarchical ray-tracers.
-- Thomas Pflaum 1996

gold = gr.material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 25)
grass = gr.material({186/255, 170/255, 156/255}, {0.0, 0.0, 0.0}, 0)
blue = gr.material({0.7, 0.6, 1}, {0.5, 0.4, 0.8}, 25, 1, 1.5)

scene = gr.node('scene')
scene:rotate('X', 23)
scene:translate(0, -2, -15)

-- -- the arc
-- arc = gr.node('arc')
-- scene:add_child(arc)
-- arc:translate(0,0,-10)
-- arc:rotate('Y', 60)
-- p1 = gr.cube('p1')
-- arc:add_child(p1)
-- p1:set_material(gold)
-- p1:scale(0.8, 4, 0.8)
-- p1:translate(-2.4, 0, -0.4)

-- p2 = gr.cube('p2')
-- arc:add_child(p2)
-- p2:set_material(gold)
-- p2:scale(0.8, 4, 0.8)
-- p2:translate(1.6, 0, -0.4)

-- s = gr.sphere('s')
-- arc:add_child(s)
-- s:set_material(gold)
-- s:scale(4, 0.6, 0.6)
-- s:translate(0, 4, 0)

-- the floor
plane = gr.mesh( 'plane', 'plane.obj' )
scene:add_child(plane)
plane:set_material(grass)
plane:scale(30, 30, 30)

-- sphere
poly = gr.cube( 'poly' )
scene:add_child(poly)
poly:scale(4, 4, 4)
poly:translate(-2, 0, -2)
-- poly:rotate('y', 45)
poly:set_material(blue)
poly:caustic()

-- The lights
l1 = gr.point_light({0,50,-40}, {0.8, 0.8, 0.8}, {1, 0, 0})
l2 = gr.point_light({0, 5, -20}, {0.5, 0.5, 0.7}, {1, 0, 0})

gr.render(scene, 'Assets/hier.png', 256, 256,
	  {0, 0, 0,}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1})
