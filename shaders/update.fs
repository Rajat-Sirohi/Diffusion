#version 330 core

uniform int N;
uniform float dt;
uniform float size;
uniform float gapSize;
uniform sampler2D positions;
uniform sampler2D velocities;

layout (location = 0) out vec4 posTexel;
layout (location = 1) out vec4 velTexel;

void main() {
    vec2 pos = texelFetch(positions,  ivec2(gl_FragCoord.xy), 0).xy;
    vec2 vel = texelFetch(velocities, ivec2(gl_FragCoord.xy), 0).xy;

    vec2 prevPos = pos;
    pos += vel * dt;

    // Resolve wall collision
    if (pos.x - size < -1) {
       pos.x = -1 + size;
       vel.x = -vel.x;
    } else if (pos.x + size > 1) {
       pos.x = 1 - size;
       vel.x = -vel.x;
    }

    if (pos.y - size < -1) {
       pos.y = -1 + size;
       vel.y = -vel.y;
    } else if (pos.y + size > 1) {
       pos.y = 1 - size;
       vel.y = -vel.y;
    }

    // Resolve barrier collision
    if (pos.x + size > 0 && pos.x - size < 0 && (pos.y + size > gapSize || pos.y - size < -gapSize)) {
       if (vel.x > 0)
          pos.x = -size;
       else
          pos.x = size;
       vel.x = -vel.x;
    } else if (prevPos.x + size < 0 && pos.x - size > 0) {
       pos.x = -pos.x;
       vel.x = -vel.x;
    }


    // Resolve particle collision
    // int index = int(gl_FragCoord.x);
    // for (int i = 0; i < N; i++) {
    //     if (i == index)
    //         continue;
        
    //     // Detect collision
    //     vec2 oPos = texelFetch(positions, ivec2(i,0), 0).xy;
    //     vec2 dist = abs(pos - oPos);
    //     float extent = size*2;
    //     bool collision = (dist.x < extent) && (dist.y < extent);
    //     if (!collision)
    //        continue;

    //     // Bounce
    //     vec2 oVel = texelFetch(velocities, ivec2(i,0), 0).xy;
    //     vel -= dot(vel - oVel, pos - oPos) / dot(pos - oPos, pos - oPos) * (pos - oPos);

    //     // Separate particle squares
    //     if (index < i) {
    //        vec2 sep = vec2(extent) - dist;
    //        if (sep.x < sep.y && sep.x > 0)
    //           sep.y = 0;
    //        else if (sep.y > 0)
    //           sep.x = 0;
    //        if (pos.x - oPos.x < 0)
    //           sep.x = -sep.x;
    //        if (pos.y - oPos.y < 0)
    //           sep.y = -sep.y;
    //        pos += sep;
    //     }
    // }
  
    posTexel = vec4(pos, 0.0f, 1.0f);
    velTexel = vec4(vel, 0.0f, 1.0f);
}
