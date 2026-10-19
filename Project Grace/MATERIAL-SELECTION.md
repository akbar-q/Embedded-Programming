# Project Grace Material Selection

## Why PETG Is the Baseline

Project Grace uses compliant arm structures to keep the driven wheels pressed against the pipe. In V7, the main arms connect to paired accordion flexures, with additional zig-zag side springs providing compliance across the frame. These features are intended to deflect under load and recover as the robot climbs; they are not merely decorative cut-outs.

The geometry was developed around PETG's balance of stiffness, toughness, and strain capacity. PETG is generally less stiff than common PLA grades and can tolerate more deformation before fracture. That makes it a practical starting material for arms that must flex repeatedly without behaving like rigid beams or breaking suddenly. It also prints with less warping than many ABS setups, which helps preserve the thin beams and their designed clearances.

This is a material-dependent mechanism. The spring dimensions, arm spacing, wheel preload, and expected deflection work together. Changing the polymer changes the force needed to move the arms, how far they move under a given load, how much they recover, and how quickly they may take a permanent set. A material substitution should therefore be treated as a mechanical redesign and validation task, not just a filament-colour change.

## How Material Changes the Flexures

For a given flexure shape, a higher elastic modulus usually means more force is needed to achieve the same deflection. A lower modulus usually means more deflection under the same force. Actual response also depends on beam geometry, print orientation, layer bonding, infill and perimeter strategy, temperature, and the specific filament formulation.

| Material | Typical behaviour relevant to this frame | Likely effect if substituted without redesign |
| --- | --- | --- |
| PETG | Moderate stiffness, comparatively ductile behaviour, and generally useful impact resistance. It can creep under sustained load, especially when warm. | Baseline response. The arms can flex while retaining useful toughness, but wheel preload and long-term sag still need testing. |
| PLA | Usually stiffer than PETG, with good dimensional accuracy but lower tolerance for repeated high strain in many grades. Heat resistance also varies and is often limited. | The arms may deflect less under the same load, changing wheel contact and increasing stress at flexure roots. Repeated cycling or an impact may initiate cracks rather than produce the intended compliant motion. |
| ABS | Often less stiff than PLA and more heat tolerant, with useful toughness when printed and bonded well. It can shrink or warp during printing, and its behaviour varies substantially by grade and process. | The frame may feel more compliant than PLA, but print distortion and different layer strength can shift alignment and preload. The spring response and fit must be measured rather than assumed to match PETG. |

These are general tendencies, not guaranteed material specifications. Check the datasheet for the exact filament and printer process; filled, modified, recycled, and high-speed formulations can behave differently from ordinary grades.

## Print Quality Is Part of the Material

The flexures are anisotropic printed parts. Their strength and fatigue life depend on the direction of the beams relative to the layers, as well as layer adhesion, nozzle temperature, cooling, wall count, and defects. A nominal PETG choice does not guarantee the intended spring behaviour if the flexure beams are poorly bonded or printed in an unfavourable orientation.

Keep the flexure roots free of sharp defects and under-extrusion. Use consistent settings across test parts, because a process change can alter stiffness and failure mode even when the CAD and filament name remain unchanged. Avoid assuming that more infill alone makes the flexures safer; the beam walls, layer direction, and local geometry are critical.

## Validation Before Use

After printing, test the frame progressively before fitting it to a person, valuable equipment, or an elevated pipe:

1. Measure the unloaded wheel spacing and compare it with the CAD target.
2. Deflect each arm through its expected operating range and check that it returns close to its original position.
3. Cycle the arms repeatedly, then inspect the flexure roots, layer interfaces, and spring connections for whitening, cracks, delamination, or permanent deformation.
4. Repeat the checks after the frame has carried its expected load for an extended period; this helps reveal creep.
5. Recheck wheel contact and motor-holder alignment after cycling.

If the design is printed in PLA, ABS, or a different PETG formulation, repeat these checks and adjust the geometry or preload as necessary. Do not assume the PETG-tuned flexure dimensions transfer unchanged.

## Design Reference

The current spring and arm geometry is defined in [ProjectGrace-SpringArm-V7.scad](ProjectGrace-SpringArm-V7.scad). This page describes the material rationale; it does not certify a load rating, fatigue life, or safe operating limit. Those values require physical testing of the selected filament, printer, orientation, and settings.
