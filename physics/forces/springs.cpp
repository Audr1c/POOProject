#include "springs.hpp"

#include "../particles.hpp"

#include <algorithm>

/*Spring::Spring(Particle &first, Particle &second, float k, float rho)
    : Spring(first, second, (second.pos - first.pos).norm(), k, rho) {}*/

Spring::Spring(Particle &first, Particle &second, float L0, float k, float rho)
    : Entity("Spring"), Force("Spring"), Drawable(), ends{&first, &second},
      L0(L0), k(k), rho(rho) {
  // S'enregistre auprès des deux particules
  Force &self = *this;
  first.acting_forces.push_back(self);
  second.acting_forces.push_back(self);
}


Vec2 Spring::compute(const Particle &particle) {
	const Vec2 displacement = ends[1]->pos - ends[0]->pos;
	const float length = displacement.norm();
	if (length == 0.0f)
		return Vec2();

	const Vec2 direction = displacement / length;
	const float relative_speed = (ends[1]->speed - ends[0]->speed).dot(direction);
	const Vec2 force_on_first =
		direction * (k * (length - L0) + rho * relative_speed);

	if (&particle == ends[0])
		return force_on_first;
	if (&particle == ends[1])
		return -force_on_first;
	return Vec2();
}

void Spring::draw(Renderer &renderer) {
  if (!visible)
    return;
  renderer.drawLine(ends[0]->pos, ends[1]->pos, stroke_color);
}
