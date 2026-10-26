# Project Grace: Presentation Content

**Format:** 16:9 widescreen, 18 slides  
**Visual direction:** Continue the poster's deep burgundy, black, signal-red and warm ivory palette, with restrained gold accents. Use the supplied CAD renders as geometry references. Generate images without text, labels, logos or invented hardware; add typography and technical annotations in PowerPoint.

---

## Slide 1 — Project Grace

### On slide
- **PROJECT GRACE**
- A compliant, motor-driven pipe climber
- **Take hold. Flex under load. Keep climbing.**
- Mechanical design | power | control | testing

### Speaker notes
Introduce Grace as a compact robot designed to travel along a vertical pipe. Its central challenge is to maintain traction while carrying the drive system, sensors and battery. The presentation follows the design from compliant frame and wheels through power, control and validation. Distinguish CAD targets and calculations from physical test results.

### Image prompt
Use the supplied Project Grace CAD render as an exact reference for shape and component placement. Create a cinematic product image of this four-wheel pipe climber gripping a vertical copper pipe, dramatic deep burgundy and black setting with controlled red highlights and warm metallic reflections. Engineering-product photography; no text, logos or added/rearranged components.

---

## Slide 2 — The Challenge

### On slide
- Climb a **22 mm outside-diameter pipe**
- Maintain wheel contact while the frame flexes
- Carry drive, sensing, wiring and battery mass
- Reverse direction and complete a controlled descent
- Keep the assembly within the **300 mm length limit**

### Speaker notes
A rigid chassis can lose wheel contact when tolerances, loading or movement disturb its geometry. Grace uses compliant arms to accommodate movement while keeping the wheels positioned around the pipe. The length limit constrains the frame and wheel envelope. The final limit check must use the actual mounted tyres, not CAD dimensions alone.

### Image prompt
Technical editorial image of a compact four-wheel robot climbing a vertical 22 mm copper pipe. Show the narrow scale and mechanical challenge; use burgundy, black and red lighting with realistic CAD-inspired forms. No text or labels.

---

## Slide 3 — System Overview

### On slide
- Four N20 geared-motor positions drive four custom wheels
- Twin rubber tyres per wheel provide **eight contact bands**
- PETG frame uses paired accordion and outer zig-zag flexures
- Limit switch and ultrasonic sensor support the programmed sequence
- Two 2S LiPo packs are planned in series as a 4S supply

### Speaker notes
The frame, motors, wheels, sensors and power source form one system. Flexures provide controlled movement, tyres contact the pipe, and the motor holders locate the drive units. The controller sketch implements the drive, limit switch, ultrasonic sensor, LEDs and buzzer. Confirm final wiring and motor-driver channel arrangement on the assembled robot. The supplied sketch does not command the chain-hook servo.

### Image prompt
Exploded technical overview using the exact supplied Project Grace CAD: V7 frame, four V6-style twin-tyre wheels, N20 motor positions, servo cradle, separate chain hook, battery packs, ultrasonic sensor and controller. Precise component placement, red and charcoal palette, no text.

---

## Slide 4 — Frame Geometry

### On slide
- V7 centre plate height: **108 mm**
- Arm length: **92 mm**
- Frame thickness: **8 mm**
- Target wheel-hub spacing: **43.2 mm**
- Approximate frame-only end span: **261 mm**

### Speaker notes
The centre plate joins four radial arms and carries electronics and mounting features. V7 calculates wheel-hub spacing as a 13.2 mm target tread gap plus two 15 mm flare allowances. The approximately 261 mm frame span is calculated from CAD arm, root and motor-pad geometry; it excludes wheels and is not a physical measurement.

### Image prompt
Clean CAD-style top view of the exact V7 chassis, showing four arms, centre plate, springs and motor pads. Use subtle dimension arrows with no numbers or text, high contrast on burgundy.

---

## Slide 5 — Controlled Compliance

### On slide
- Paired accordion flexures sit at the arm roots
- Flexure length: **28 mm**; beam width: **3.5 mm**
- Paired beams are spaced **16 mm** apart to resist twist
- Outer zig-zag springs use **4 mm** beams and **4 folds**
- Flex where movement helps; reinforce where loads concentrate

### Speaker notes
The paired arm-root flexures let the wheels move relative to the centre body. Their spacing helps resist unwanted twist while allowing movement along the intended compliance direction. Outer zig-zag springs add lateral compliance. These are geometric design values, not a fatigue-life guarantee; print orientation, layer bonding, material and loading all affect performance.

### Image prompt
Close-up engineering render of the exact paired accordion flexures and outer zig-zag springs in the V7 CAD. Show subtle arrows for intended deflection, red PETG and visible layer lines; no labels or extra parts.

---

## Slide 6 — Geometry Calculation: Arm Angle

### On slide
- Hub spacing target: **43.2 mm**
- Arm-root horizontal coordinate: **12 mm**
- Arm length: **92 mm**

$$
\theta=\cos^{-1}\left(\frac{43.2/2-12}{92}\right)\approx84.0^\circ
$$

### Speaker notes
The CAD derives the arm angle from target hub spacing, root position and arm length. The cosine term is 9.6 divided by 92, approximately 0.1043, giving an angle close to 84 degrees. This is a geometry calculation. Check the printed hub locations for shrinkage and assembly offsets.

### Image prompt
Minimal engineering diagram of a chassis arm showing root, angled 92 mm arm and wheel hub position. Include blank dimension arrows for PowerPoint annotation, ivory linework on burgundy, no text or numbers.

---

## Slide 7 — Frame Span and Wheel Envelope

### On slide
- Current V7 frame span: approximately **260 mm**, excluding wheels
- Wheel-centre span along the long axis: approximately **257 mm**
- CAD tyre-seat ledge diameter: **31.88 mm**
- Estimated outer wheel envelope: approximately **289 mm**
- Verify the real rubber tyre diameter before claiming limit compliance

### Speaker notes
The span estimate uses the current 108 mm centre height, arm roots, approximately 84-degree arm angle and 36 mm motor-pad width. Adding a nominal 31.88 mm wheel diameter to the estimated 257 mm wheel-centre span gives about 289 mm. This leaves roughly 11 mm under a 300 mm limit only if the installed tyre outside diameter matches the assumed dimension. Measure the assembled prototype at its widest points.

### Image prompt
Top-view CAD silhouette with dimension arrows indicating frame end span and wheel-to-wheel envelope, exact V7 arm layout and twin tyres; neutral technical background and no labels or generated numbers.

---

## Slide 8 — Custom Wheel and N20 Tyres

### On slide
- Custom wheel reuses the supplied N20 rubber tyres
- Two **6.5 mm** tyre seats; **9 mm** centre waist
- **22 mm** total wheel width
- **31.88 mm** retaining-ledge diameter
- Five split spokes connect hub to a continuous outer rim

### Speaker notes
The printed wheel combines the supplied rubber tyres with a custom hub and support structure. Each tyre sits on an outer land and raised ledge; the waist separates the twin tracks and clears the pipe. Five split spokes reduce material while joining the centre hub to a continuous rim. Verify tyre seating, alignment and resistance to movement on the actual print.

### Image prompt
Accurate three-quarter CAD render of the V6 twin-tyre wheel: five split spokes, continuous rim, twin N-profile lands, retaining lips and D-bore. Isolated on burgundy; do not alter the geometry or add text.

---

## Slide 9 — Torque Transfer and Offset

### On slide
- N20 shaft: **2.85 mm** nominal D-shaft
- CAD bore: **3.10 mm** across its nominal diameter
- V6 offset boss: **9.2 mm diameter × 2.5 mm projection**
- Boss positions the wheel away from the frame
- Tyre adhesive transfers drive torque into the rubber

### Speaker notes
The D-shaped bore transfers motor rotation to the printed wheel. A 0.25 mm diametral allowance gives a nominal 3.10 mm bore; printed fit depends on process and material. The short boss creates a stand-off from the frame. The rubber tyre is bonded to the printed seat, so both the shaft fit and tyre bond should be checked under load.

### Image prompt
Exploded macro rendering: N20 D-shaft, V6 wheel with visible one-sided 2.5 mm boss, and twin rubber tyres on their lands. Show alignment and engagement clearly; no labels, no invented hardware.

---

## Slide 10 — Motor Cradle

### On slide
- N20 envelope: **45.7 × 15.23 × 11.82 mm**
- Clear holder length: **47.7 mm**
- Full-length floor and solid side walls
- Half-height gearbox guides; open top with partial roof
- Raised inboard tie tabs avoid the wheel end

### Speaker notes
The holder is dimensioned around the modeled motor envelope and provides a nominal 2 mm length allowance. It has a full-length floor, solid side walls, half-height gearbox guides and a partial roof. Raised Velcro tabs are placed toward the inboard end to reduce interference with wheel rotation. Check the physical gearbox shape and insertion path using a test print.

### Image prompt
Accurate close CAD render of the V7 motor holder with N20 motor shown above the open top, partial roof, solid walls, gearbox guides and raised inboard Velcro tabs. No text; preserve geometry.

---

## Slide 11 — Servo and Chain Hook

### On slide
- SG90-compatible horn with an **8 mm** hub
- Hook centre **20 mm** from servo axis
- Hook radii: **8 mm outer**, **4.5 mm inner**
- Hook mouth: **6 mm**; arm width: **5 mm**
- Servo/hook actuation is a mechanical feature, not in the supplied control sketch

### Speaker notes
The separate chain-hook horn mounts to an SG90-compatible spline and centre screw. Its 20 mm reach positions the open hook to engage the chain. The spline should be fit-tested with a short print before full fabrication. The current controller firmware contains no servo command sequence, so servo movement and hook timing remain to be integrated and tested.

### Image prompt
Exact CAD-style chain-hook horn next to an SG90 micro servo and chain link, showing the 20 mm arm and open hook. Dramatic red/black lighting; no text or extra mechanisms.

---

## Slide 12 — PETG and Flexure Behaviour

### On slide
- Flexures are designed for repeated movement
- PETG balances stiffness, toughness and strain capacity
- PLA may be stiffer and less tolerant of repeated high strain
- ABS may behave differently and can warp during printing
- Changing material changes the spring system

### Speaker notes
The flexure geometry was developed around PETG’s mechanical response. Material affects force-deflection behaviour, recovery and fatigue. PLA is often stiffer but may be more crack-prone under repeated high strain; ABS may be tougher but can warp and alter alignment. These are general tendencies: exact filament grade, print orientation and settings matter. Substitutions require physical testing and may require geometry changes.

### Image prompt
Red printed PETG accordion flexure shown under gentle controlled deflection with subtle PETG, PLA and ABS material swatches. Technical macro photography, realistic layer lines, no text and no fracture imagery.

---

## Slide 13 — Bonded Assembly

### On slide
- VHB-style tape secures lightweight battery packs
- Flexible gel adhesive bonds rubber tyres to printed wheels
- Bonding can reduce cradles, brackets, print mass and print time
- Surface preparation and full cure are essential
- Validate shear, peel and vibration resistance

### Speaker notes
Direct bonding is a mass- and material-saving design choice. VHB tape provides a broad compliant bond for the battery; gel superglue is selected for stated rubber compatibility. Surface cleanliness, contact area, adhesive compatibility and cure time determine the real joint strength. Motor torque produces repeated shear at the tyre bond, while exposed edges can experience peel. Product claims do not replace tests on the actual tyre and print.

### Image prompt
Workshop close-up of a rubber tyre bonded onto a red printed wheel with controlled gel adhesive and a lightweight LiPo attached to a PETG plate with double-sided tape. No branding or text.

---

## Slide 14 — Battery System

### On slide
- Two **2S, 450 mAh** LiPo packs connected in series
- Combined supply: **4S, 14.8 V nominal; 16.8 V full**
- Series capacity remains **450 mAh**
- Approximate pair mass: **44 g**
- Pack advertises **60C**; verify rating and current capability

### Speaker notes
Series connection adds voltage, not capacity. Two 7.4 V nominal 2S packs produce 14.8 V nominal and 16.8 V fully charged; capacity remains 450 mAh. At approximately 22 g each, the pair weighs around 44 g before wiring. The label advertises 60C, which would imply 27 A at 0.45 Ah if the rating were achieved: `0.45 × 60 = 27 A`. Treat this as a label-based theoretical value, not a verified continuous rating. Confirm controller voltage limits and use an appropriate 4S balance-charging method.

### Image prompt
Two compact 2S LiPo packs joined in series beside the climber, clearly show red/black wiring and lightweight form factor, technical product photography on dark burgundy, no brand names or generated labels.

---

## Slide 15 — Power and Runtime Estimate

### On slide
- Estimated system draw: **25–45 W**
- At 14.8 V: approximately **1.69–3.04 A**
- Nominal pack energy: **6.66 Wh**
- Ideal runtime: **16 min at 25 W; 8.9 min at 45 W**
- Practical runtime is lower due to reserve, losses and load peaks

### Speaker notes
Nominal energy is `14.8 V × 0.45 Ah = 6.66 Wh`. Current estimates use `I = P/V`, giving about 1.69 A at 25 W and 3.04 A at 45 W. Ideal runtime is `energy/power`: approximately 0.266 hours (16 minutes) at 25 W and 0.148 hours (8.9 minutes) at 45 W. At 80% usable capacity, estimates fall to about 12.8 and 7.1 minutes. These are planning calculations; measure actual current and runtime during climbing.

### Image prompt
Technical energy-flow graphic showing 4S battery feeding motor controller and four drive motors, use abstract meter arcs and blank callout areas for PowerPoint data; burgundy, red, ivory, no generated text.

---

## Slide 16 — Controller Sequence

### On slide
1. Ascend at PWM command **190/255**
2. Limit switch triggers a stop
3. Wait **11 s**, then begin descent
4. Stop at ultrasonic reading **110 cm**
5. Pause **2 s**, continue to **200 cm**, then stop

### Speaker notes
The firmware uses a finite-state machine. A low limit-switch input ends ascent. Descent begins after an 11-second wait. The ultrasonic sensor is read every 100 ms with a 30 ms echo timeout. The controller pauses at the 110 cm threshold for two seconds, then continues until the reading reaches 200 cm. These are sensor-distance thresholds, not measured vertical travel distances. Validate sensor orientation and target geometry on the prototype.

### Image prompt
State-machine infographic with icons for ascent, limit switch, wait, ultrasonic midpoint, pause, final descent and completion. Leave labels blank for PowerPoint; use deep burgundy, red and ivory.

---

## Slide 17 — Safety and Validation

### On slide
- Confirm real tyre diameter and total assembled envelope
- Test motor fit, wheel spacing and battery retention
- Cycle flexures; inspect roots and layer bonds
- Test tyre adhesive under torque and vibration
- Verify sensor thresholds, current and runtime
- CAD calculations are targets, not safety certification

### Speaker notes
V7 geometry gives an estimated frame-only span of about 259.5 mm. The wheel-centre span is about 257 mm; adding a nominal 31.88 mm tyre-seat diameter suggests an outer envelope near 289 mm. That leaves about 11 mm under 300 mm only if the mounted rubber tyres match the assumed diameter. Measure the assembled robot. Test progressively with restraints and inspect the structure, bonds, wiring and battery after repeated cycles. No rated load or safe operating limit is established by CAD alone.

### Image prompt
Engineering test scene: Project Grace restrained on a vertical test pipe, technician using calipers to check tyre envelope and inspecting PETG flexures and adhesive joints, safe controlled workshop, red and burgundy palette, no text.

---

## Slide 18 — Design Philosophy

### On slide
- Flex where movement helps
- Reinforce where loads concentrate
- Treat geometry, material and assembly as one system
- Remove unnecessary mass without removing essential load paths
- **Take hold. Do not yield. Rise.**

### Speaker notes
Grace is neither entirely rigid nor freely flexible. The arms and springs are intended to yield in controlled regions so the wheels can maintain contact; the centre body, mounts and hubs carry the load. PETG response, print orientation, tyre retention, adhesives and control timing all influence system behaviour. Every prototype test should inform the next measured revision.

### Image prompt
Hero image using the supplied Project Grace CAD as exact geometry reference, robot gripping and climbing a vertical copper pipe into a dark burgundy industrial space, red light tracing arms and wheels, credible engineering mood, no text or added components.

---

## Slide 19 — Closing: The Rite of Ascent

### On slide
**Take hold.**  
**Do not yield.**  
**Rise.**

### Speaker notes
Close by returning to the project’s central idea: reliable climbing comes from coordinated design. The frame flexes deliberately, the tyres grip, the drive supplies motion and the controller guides the sequence. The next milestone is not another claim—it is a repeatable, instrumented climb with measured loads, runtime and inspection results.

### Image prompt
Vertical cinematic view of the exact Project Grace climber ascending a copper pipe, red reflections against near-black burgundy, restrained dramatic lighting, clear silhouette, no text, logos or invented hardware.

---

## Presenter Accuracy Notes
- V7 CAD: centre plate **108 mm**, arms **92 mm**, frame thickness **8 mm**, target hub spacing **43.2 mm**.
- V7 motor cradle interior: **47.7 mm**; motor envelope in CAD: **45.7 × 15.23 × 11.82 mm**.
- V6 wheel: **22 mm** wide, **31.88 mm** ledge diameter, **2.85 mm** nominal D-shaft with **0.25 mm** CAD clearance, **2.5 mm** offset boss.
- Estimated frame-only span: approximately **259.5 mm**; estimated total span with nominal **31.88 mm** tyres: approximately **289 mm**. Measure actual mounted rubber tyres before claiming the 300 mm limit is met.
- The controller code implements ascent/descent with limit and ultrasonic inputs, but does **not** implement servo control for the chain hook.
- Battery figures, power draw, adhesive suitability and material behaviour are based on project-provided specifications or estimates. Verify against component labels, manufacturer data and physical tests.
- The project’s supplied 2S packs are described as 450 mAh, 60C and approximately 22 g each. The arithmetic `0.45 Ah × 60C = 27 A` is a label-based theoretical figure, not an independently verified continuous current rating.
