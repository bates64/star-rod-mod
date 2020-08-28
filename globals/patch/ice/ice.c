#include "common.h"

#define ACCELERATION 0.07f
#define FRICTION_ICE 0.98f
#define FRICTION     0.75f
#define MAX_VELOCITY gPlayerStatus.runSpeed
#define SPIN_SPEED   2.0f

f32 _xv    = 0.0f;
f32 _zv    = 0.0f;
u8 _on_ice = FALSE;

s32 test_ray_colliders(u32 ignore_flags, f32 start_x, f32 start_y, f32 start_z, f32 dir_x, f32 dir_y, f32 dir_z, f32* hit_x, f32* hit_y, f32* hit_z, f32* hit_depth, f32* hit_nx, f32* hit_ny, f32* hit_nz);
s32 get_collider_type_by_id(s32 id);
s32 do_lateral_collision(s32 type, PlayerStatus* player_status, f32* x, f32* y, f32* z, f32 depth, f32 yaw);
s32 test_player_lateral(PlayerStatus* player, f32* x, f32* y, f32* z, f32 distance, f32 yaw);

void input_to_move_vector(f32* angle, f32* speed);

void sin_cos_deg(f32 theta, f32* sin, f32* cos);
f32 get_clamped_angle_diff(f32 a, f32 b);

s8 _clamp(s8 value, s8 min, s8 max, s8 deadzone) {
	if (value > 0 && value < deadzone) return 0;
	if (value < 0 && value > -deadzone) return 0;

	if (value < min) return min;
	if (value > max) return max;

	return value;
}

f32 _abs(f32 value) {
	return value > 0 ? value : -value;
}

void _apply_input(u8 no_interpolate) {
	f32 heading, magnitude;
	f32 target_xv, target_zv;

	input_to_move_vector(&heading, &magnitude);
	magnitude /= 70.0f;

	sin_cos_deg(heading, &target_xv, &target_zv);
	target_xv *= magnitude * MAX_VELOCITY;
	target_zv *= magnitude * MAX_VELOCITY * -1;

	if (magnitude) {
		if (no_interpolate) {
			_xv = target_xv;
			_zv = target_zv;
		} else {
			_xv += (target_xv - _xv) * ACCELERATION;
			_zv += (target_zv - _zv) * ACCELERATION;
		}
	} else {
		_xv *= FRICTION_ICE;
		if (_abs(_xv) < 0.01f) _xv = 0;

		_zv *= FRICTION_ICE;
		if (_abs(_zv) < 0.01f) _zv = 0;
	}

	if (_xv > MAX_VELOCITY) _xv *= 0.9f;
	if (_xv < -MAX_VELOCITY) _xv *= 0.9f;

	if (_zv > MAX_VELOCITY) _zv *= 0.9f;
	if (_zv < -MAX_VELOCITY) _zv *= 0.9f;
}

u8 ice_physics_test(void) {
	CollisionStatus gCollisionStatus = *(CollisionStatus*)0x8015A550;
	s16 collider = gCollisionStatus.currentFloor;

	if (collider == -1) return _on_ice;

	return get_collider_type_by_id(collider) & 0x00000010; // new 'is ice' flag
}

void ice_physics_main(void) {
	f32 xv, zv;
	u32 gPlayerInputFlags = *(u32*)0x8010EFC8;

	if (gPlayerInputFlags & 0x00002000) {
		_xv = 0.0f;
		_zv = 0.0f;
		return;
	}

	if (_abs(_xv) < 0.03f) _xv = 0;
	if (_abs(_zv) < 0.03f) _zv = 0;

	if (ice_physics_test()) {
		// On ice.

		if (
			!_on_ice || // First frame
			gPlayerStatus.actionState == ActionState_IDLE ||
			gPlayerStatus.actionState == ActionState_WALK ||
			gPlayerStatus.actionState == ActionState_RUN
		) {
			// On first frame of ice, accelerate to full speed immediately. This makes walking onto ice not kill your velocity.
			_apply_input(!_on_ice);
		}

		_on_ice = TRUE;
	} else {
		// We left ice, deccelerate quickly.

		_xv *= FRICTION;
		_zv *= FRICTION;

		_on_ice = FALSE;
	}

	if (test_player_lateral(
		&gPlayerStatus,
		&gPlayerStatus.position.x, &gPlayerStatus.position.y, &gPlayerStatus.position.z,
		dist2D(0.0f, 0.0f, _xv, _zv) * (gPlayerStatus.actionState == ActionState_SPIN ? SPIN_SPEED : 1.0f),
		atan2(0.0f, 0.0f, _xv, _zv)
	) == -1) {
		// Movement done
	} else {
		// Hit a wall
		_xv *= FRICTION;
		_zv *= FRICTION;
	}
}
