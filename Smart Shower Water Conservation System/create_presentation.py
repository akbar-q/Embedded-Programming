from pathlib import Path

from pptx import Presentation
from pptx.dml.color import RGBColor
from pptx.enum.shapes import MSO_AUTO_SHAPE_TYPE, MSO_CONNECTOR
from pptx.enum.text import MSO_ANCHOR, PP_ALIGN
from pptx.util import Inches, Pt


OUTPUT_FILE = Path(__file__).with_name("Smart_Shower_Water_Conservation_System.pptx")

NAVY = RGBColor(12, 35, 57)
TEAL = RGBColor(0, 153, 153)
CYAN = RGBColor(0, 196, 222)
SKY = RGBColor(218, 245, 250)
MINT = RGBColor(210, 243, 229)
GREEN = RGBColor(34, 174, 102)
AMBER = RGBColor(244, 172, 52)
ORANGE = RGBColor(239, 116, 45)
RED = RGBColor(211, 62, 71)
BLUE = RGBColor(55, 122, 231)
INK = RGBColor(28, 50, 66)
SLATE = RGBColor(94, 112, 124)
WHITE = RGBColor(255, 255, 255)
LIGHT = RGBColor(246, 250, 251)
LINE = RGBColor(207, 223, 229)


def set_fill(shape, color):
    shape.fill.solid()
    shape.fill.fore_color.rgb = color
    shape.line.fill.background()


def add_text(slide, text, x, y, w, h, size=18, color=INK, bold=False,
             font="Aptos", align=PP_ALIGN.LEFT, valign=MSO_ANCHOR.TOP):
    box = slide.shapes.add_textbox(Inches(x), Inches(y), Inches(w), Inches(h))
    frame = box.text_frame
    frame.clear()
    frame.word_wrap = True
    frame.vertical_anchor = valign
    paragraph = frame.paragraphs[0]
    paragraph.text = text
    paragraph.alignment = align
    run = paragraph.runs[0]
    run.font.name = font
    run.font.size = Pt(size)
    run.font.bold = bold
    run.font.color.rgb = color
    return box


def add_rect(slide, x, y, w, h, color, radius=False, line_color=None):
    shape_type = MSO_AUTO_SHAPE_TYPE.ROUNDED_RECTANGLE if radius else MSO_AUTO_SHAPE_TYPE.RECTANGLE
    shape = slide.shapes.add_shape(shape_type, Inches(x), Inches(y), Inches(w), Inches(h))
    set_fill(shape, color)
    if line_color:
        shape.line.color.rgb = line_color
    return shape


def add_circle(slide, x, y, d, color):
    shape = slide.shapes.add_shape(MSO_AUTO_SHAPE_TYPE.OVAL, Inches(x), Inches(y), Inches(d), Inches(d))
    set_fill(shape, color)
    return shape


def add_line(slide, x1, y1, x2, y2, color=LINE, width=1.4, arrow=False):
    line = slide.shapes.add_connector(
        MSO_CONNECTOR.STRAIGHT, Inches(x1), Inches(y1), Inches(x2), Inches(y2)
    )
    line.line.color.rgb = color
    line.line.width = Pt(width)
    if arrow:
        line.line.end_arrowhead = True
    return line


def add_header(slide, number, kicker, title, subtitle=None):
    add_text(slide, f"{number:02d}", 0.55, 0.38, 0.55, 0.28, 11, TEAL, True, align=PP_ALIGN.CENTER)
    add_text(slide, kicker.upper(), 1.2, 0.38, 2.8, 0.28, 10, TEAL, True)
    add_text(slide, title, 0.55, 0.77, 12.0, 0.52, 28, NAVY, True, "Aptos Display")
    if subtitle:
        add_text(slide, subtitle, 0.57, 1.34, 11.9, 0.35, 12, SLATE)
    add_line(slide, 0.55, 1.78, 12.75, 1.78, LINE, 0.8)


def add_footer(slide, text="SMART SHOWER WATER CONSERVATION SYSTEM"):
    add_line(slide, 0.55, 7.08, 12.75, 7.08, LINE, 0.8)
    add_text(slide, text, 0.55, 7.17, 5.5, 0.2, 8, SLATE, True)
    add_text(slide, "FOAM-BOARD COMPETITION PROTOTYPE", 8.2, 7.17, 4.55, 0.2, 8, SLATE, True, align=PP_ALIGN.RIGHT)


def add_bullet(slide, text, x, y, w, color=INK, marker=TEAL):
    add_circle(slide, x, y + 0.1, 0.11, marker)
    add_text(slide, text, x + 0.22, y, w - 0.22, 0.5, 13, color)


def new_slide(prs):
    slide = prs.slides.add_slide(prs.slide_layouts[6])
    background = slide.background.fill
    background.solid()
    background.fore_color.rgb = WHITE
    return slide


def add_title_slide(prs):
    slide = new_slide(prs)
    add_rect(slide, 0, 0, 13.333, 7.5, NAVY)
    add_circle(slide, 9.4, -1.55, 5.4, TEAL)
    add_circle(slide, 10.65, 1.0, 3.5, CYAN)
    add_circle(slide, 8.65, 4.65, 2.2, RGBColor(22, 76, 99))
    add_text(slide, "SMART SHOWER", 0.72, 0.76, 7.4, 0.52, 13, CYAN, True)
    add_text(slide, "Water Conservation\nSystem", 0.7, 1.25, 7.5, 1.55, 38, WHITE, True, "Aptos Display")
    add_text(slide, "A responsive foam-board prototype that makes shower demand visible and encourages lower water and energy use.", 0.74, 3.15, 6.2, 0.75, 16, RGBColor(212, 231, 238))
    add_rect(slide, 0.72, 4.55, 2.05, 0.55, TEAL, True)
    add_text(slide, "ESP32 WROOM-32D", 0.84, 4.7, 1.82, 0.2, 10, WHITE, True, align=PP_ALIGN.CENTER)
    add_rect(slide, 2.95, 4.55, 1.68, 0.55, RGBColor(31, 93, 120), True)
    add_text(slide, "LIVE DEMO", 3.05, 4.7, 1.48, 0.2, 10, WHITE, True, align=PP_ALIGN.CENTER)

    # Compact board illustration.
    add_rect(slide, 8.15, 1.58, 3.85, 4.48, RGBColor(235, 244, 245), True)
    add_text(slide, "FOAM-BOARD CONTROL PANEL", 8.45, 1.84, 3.25, 0.25, 9, NAVY, True, align=PP_ALIGN.CENTER)
    add_circle(slide, 8.72, 2.47, 0.85, ORANGE)
    add_circle(slide, 10.55, 2.47, 0.85, BLUE)
    add_text(slide, "HOT", 8.7, 3.42, 0.9, 0.22, 9, NAVY, True, align=PP_ALIGN.CENTER)
    add_text(slide, "COLD", 10.5, 3.42, 0.95, 0.22, 9, NAVY, True, align=PP_ALIGN.CENTER)
    for index, colour in enumerate((GREEN, GREEN, AMBER, AMBER, ORANGE, RED, RED, RED)):
        add_circle(slide, 8.63 + index * 0.38, 4.18, 0.23, colour)
    add_text(slide, "status ring", 8.55, 4.55, 3.0, 0.25, 9, SLATE, align=PP_ALIGN.CENTER)
    add_rect(slide, 8.72, 5.02, 2.82, 0.48, NAVY, True)
    add_text(slide, "TIMED FLOW CONTROL", 8.85, 5.16, 2.56, 0.2, 9, WHITE, True, align=PP_ALIGN.CENTER)
    add_text(slide, "Competition presentation", 0.72, 6.72, 4.0, 0.25, 10, RGBColor(160, 192, 204), True)
    return slide


def add_problem_slide(prs):
    slide = new_slide(prs)
    add_header(slide, 2, "The challenge", "Water waste is often invisible in the moment", "The prototype makes time, demand, and conservation feedback easy to see.")
    cards = [
        ("01", "Long sessions", "The user has little live feedback while time and water use keep increasing.", TEAL),
        ("02", "Hot-water energy", "Higher hot demand also represents energy used to heat the water.", ORANGE),
        ("03", "Late intervention", "A sudden shutoff is frustrating. Early prompts allow behaviour to change first.", RED),
    ]
    for index, (number, heading, body, accent) in enumerate(cards):
        x = 0.7 + index * 4.18
        add_rect(slide, x, 2.25, 3.75, 2.9, LIGHT, True, LINE)
        add_rect(slide, x, 2.25, 3.75, 0.12, accent)
        add_text(slide, number, x + 0.28, 2.65, 0.5, 0.3, 17, accent, True)
        add_text(slide, heading, x + 0.28, 3.1, 3.1, 0.35, 18, NAVY, True, "Aptos Display")
        add_text(slide, body, x + 0.28, 3.66, 3.05, 0.9, 13, SLATE)
    add_rect(slide, 0.7, 5.72, 11.95, 0.75, SKY, True)
    add_text(slide, "Design principle: inform first, then restrict gradually only when the configured limit is exceeded.", 0.95, 5.96, 11.4, 0.22, 15, NAVY, True, align=PP_ALIGN.CENTER)
    add_footer(slide)
    return slide


def add_prototype_slide(prs):
    slide = new_slide(prs)
    add_header(slide, 3, "Prototype", "A safe, visual model of a smart shower", "This is a foam-board simulation. It demonstrates logic and interaction, not real plumbing control.")
    components = [
        (1.0, 2.35, "HOT\nPOT", ORANGE, "Sets hot flow"),
        (3.38, 2.35, "COLD\nPOT", BLUE, "Sets cold flow"),
        (5.78, 2.35, "ESP32", NAVY, "Calculates demand"),
        (8.18, 2.35, "LED\nFEEDBACK", TEAL, "Shows status"),
        (10.58, 2.35, "GAUGES", GREEN, "Shows flow"),
    ]
    for index, (x, y, label, colour, caption) in enumerate(components):
        add_circle(slide, x, y, 1.2, colour)
        add_text(slide, label, x + 0.08, y + 0.38, 1.04, 0.38, 12, WHITE, True, align=PP_ALIGN.CENTER, valign=MSO_ANCHOR.MIDDLE)
        add_text(slide, caption, x - 0.22, y + 1.42, 1.65, 0.28, 10, SLATE, align=PP_ALIGN.CENTER)
        if index < len(components) - 1:
            add_line(slide, x + 1.25, y + 0.6, x + 2.1, y + 0.6, TEAL, 2.0, True)
    add_rect(slide, 1.0, 5.1, 11.35, 0.92, LIGHT, True, LINE)
    add_text(slide, "Two potentiometers model the tap settings. The ESP32 blends them into a simulated temperature, demand level, conservation score, light feedback, and servo gauge positions.", 1.32, 5.37, 10.7, 0.3, 13, INK, align=PP_ALIGN.CENTER)
    add_footer(slide)
    return slide


def add_algorithm_slide(prs):
    slide = new_slide(prs)
    add_header(slide, 4, "Control loop", "The decision-making sequence runs continuously", "Each control update is fast, non-blocking, and visible on the prototype.")
    steps = [
        ("Read", "Hot + cold\ncontrols", CYAN),
        ("Calculate", "Demand +\ntemperature", TEAL),
        ("Assess", "Time limit +\nscore", AMBER),
        ("Respond", "LEDs +\ngauges", GREEN),
        ("Restrict", "If over\nlimit", RED),
    ]
    for index, (title, body, colour) in enumerate(steps):
        x = 0.7 + index * 2.48
        add_rect(slide, x, 2.45, 2.05, 2.0, LIGHT, True, LINE)
        add_circle(slide, x + 0.72, 2.72, 0.62, colour)
        add_text(slide, str(index + 1), x + 0.72, 2.88, 0.62, 0.2, 11, WHITE, True, align=PP_ALIGN.CENTER)
        add_text(slide, title, x + 0.2, 3.55, 1.65, 0.25, 15, NAVY, True, align=PP_ALIGN.CENTER)
        add_text(slide, body, x + 0.16, 3.94, 1.73, 0.36, 11, SLATE, align=PP_ALIGN.CENTER)
        if index < len(steps) - 1:
            add_line(slide, x + 2.08, 3.45, x + 2.38, 3.45, TEAL, 1.8, True)
    add_rect(slide, 2.36, 5.28, 8.55, 0.6, SKY, True)
    add_text(slide, "The system avoids abrupt behaviour: it gives clear feedback before applying any simulated reduction.", 2.63, 5.48, 8.0, 0.2, 13, NAVY, True, align=PP_ALIGN.CENTER)
    add_footer(slide)
    return slide


def add_feedback_slide(prs):
    slide = new_slide(prs)
    add_header(slide, 5, "Live feedback", "One glance communicates the current state", "The LEDs provide a readable signal while the gauges show controlled hot and cold flow.")
    states = [
        (GREEN, "Efficient", "Within target"),
        (AMBER, "Caution", "Approaching limit"),
        (ORANGE, "Warning", "Reduce demand"),
        (BLUE, "Pause", "Soap or shampoo"),
        (RED, "Limit", "Restriction active"),
    ]
    for index, (colour, label, meaning) in enumerate(states):
        x = 0.72 + index * 2.52
        add_circle(slide, x + 0.63, 2.54, 0.75, colour)
        add_text(slide, label, x, 3.54, 2.0, 0.28, 14, NAVY, True, align=PP_ALIGN.CENTER)
        add_text(slide, meaning, x, 3.93, 2.0, 0.25, 10, SLATE, align=PP_ALIGN.CENTER)
    add_rect(slide, 1.15, 5.05, 4.75, 0.9, LIGHT, True, LINE)
    add_text(slide, "TEMPERATURE BAR", 1.42, 5.25, 1.65, 0.2, 10, TEAL, True)
    for index in range(8):
        colour = BLUE if index < 3 else ORANGE
        add_rect(slide, 3.15 + index * 0.28, 5.25, 0.2, 0.28, colour, True)
    add_text(slide, "Simulated mixed-water temperature", 1.42, 5.58, 4.1, 0.18, 10, SLATE)
    add_rect(slide, 7.35, 5.05, 4.75, 0.9, LIGHT, True, LINE)
    add_text(slide, "FLOW GAUGES", 7.62, 5.25, 1.35, 0.2, 10, TEAL, True)
    add_line(slide, 9.63, 5.54, 10.65, 5.18, ORANGE, 2.2)
    add_line(slide, 10.93, 5.54, 11.55, 5.35, BLUE, 2.2)
    add_text(slide, "Separate hot and cold flow indications", 7.62, 5.58, 4.1, 0.18, 10, SLATE)
    add_footer(slide)
    return slide


def add_timeline_slide(prs):
    slide = new_slide(prs)
    add_header(slide, 6, "Conservation sequence", "A staged response rewards earlier action", "Short thresholds are used for a competition demonstration and can be extended for normal use.")
    stages = [
        ("0-45 sec", "EFFICIENT", "100% flow", GREEN),
        ("45-60 sec", "CAUTION", "80% flow", AMBER),
        ("60-75 sec", "WARNING", "80% to 40%", ORANGE),
        ("75-95 sec", "LIMIT", "40% to 0%", RED),
        ("95 sec+", "STOP", "0% flow", NAVY),
    ]
    widths = [3.45, 1.7, 1.95, 2.4, 2.0]
    current_x = 0.68
    for (time, state, flow, colour), width in zip(stages, widths):
        add_rect(slide, current_x, 2.78, width, 1.62, colour, True)
        add_text(slide, time, current_x + 0.18, 3.08, width - 0.36, 0.22, 11, WHITE, True, align=PP_ALIGN.CENTER)
        add_text(slide, state, current_x + 0.16, 3.46, width - 0.32, 0.26, 15, WHITE, True, align=PP_ALIGN.CENTER)
        add_text(slide, flow, current_x + 0.16, 3.83, width - 0.32, 0.22, 11, WHITE, align=PP_ALIGN.CENTER)
        current_x += width + 0.08
    add_text(slide, "At every stage, the system makes the change obvious through LEDs, gauges, and shower animation speed.", 1.3, 5.1, 10.7, 0.32, 14, INK, True, align=PP_ALIGN.CENTER)
    add_footer(slide)
    return slide


def add_technical_slide(prs):
    slide = new_slide(prs)
    add_header(slide, 7, "Technical implementation", "The final prototype wiring is stable and calibrated", "All values below correspond to the current ESP32 firmware.")
    left = [
        ("Hot potentiometer", "GPIO34 | ADC1 input"),
        ("Cold potentiometer", "GPIO15 | calibrated ADC input"),
        ("Hot-side gauge servo", "GPIO33 | PWM output"),
        ("Cold-side gauge servo", "GPIO25 | PWM output"),
    ]
    right = [
        ("Hot ADC range", "0 to 3604"),
        ("Cold ADC range", "0 to 3610"),
        ("WS2812 outputs", "GPIO18 and GPIO17"),
        ("Servo supply", "External 5 V + common GND"),
    ]
    for column, rows, base_x in (("WIRING", left, 0.8), ("CALIBRATION", right, 6.92)):
        add_text(slide, column, base_x, 2.22, 2.5, 0.22, 11, TEAL, True)
        for index, (name, value) in enumerate(rows):
            y = 2.62 + index * 0.82
            add_rect(slide, base_x, y, 5.62, 0.62, LIGHT, True, LINE)
            add_text(slide, name, base_x + 0.2, y + 0.18, 2.8, 0.18, 11, NAVY, True)
            add_text(slide, value, base_x + 3.0, y + 0.18, 2.35, 0.18, 10, SLATE, align=PP_ALIGN.RIGHT)
    add_rect(slide, 0.8, 6.17, 11.73, 0.5, SKY, True)
    add_text(slide, "Calibration is transparent: Serial Monitor reports both raw ADC values at 115200 baud for repeatable tuning.", 1.1, 6.33, 11.1, 0.18, 11, NAVY, True, align=PP_ALIGN.CENTER)
    add_footer(slide)
    return slide


def add_close_slide(prs):
    slide = new_slide(prs)
    add_rect(slide, 0, 0, 13.333, 7.5, NAVY)
    add_circle(slide, 9.75, -1.65, 4.6, TEAL)
    add_circle(slide, 10.5, 4.9, 2.1, RGBColor(32, 83, 104))
    add_text(slide, "THE RESULT", 0.78, 0.92, 2.3, 0.26, 11, CYAN, True)
    add_text(slide, "A smart shower concept\nthat makes conservation visible.", 0.75, 1.42, 7.35, 1.35, 31, WHITE, True, "Aptos Display")
    points = [
        "Interactive hot and cold controls",
        "Clear staged conservation feedback",
        "ESP32-controlled, safe foam-board demonstration",
    ]
    for index, point in enumerate(points):
        add_circle(slide, 0.83, 3.55 + index * 0.55, 0.12, CYAN)
        add_text(slide, point, 1.08, 3.47 + index * 0.55, 5.8, 0.28, 15, RGBColor(222, 237, 242))
    add_rect(slide, 8.05, 2.23, 3.65, 2.35, RGBColor(235, 244, 245), True)
    add_text(slide, "SMART\nSHOWER", 8.38, 2.68, 2.98, 0.64, 23, NAVY, True, "Aptos Display", PP_ALIGN.CENTER)
    add_text(slide, "Water. Energy. Awareness.", 8.28, 3.62, 3.18, 0.25, 11, TEAL, True, align=PP_ALIGN.CENTER)
    add_text(slide, "Thank you", 0.76, 6.58, 2.0, 0.3, 16, WHITE, True)
    return slide


def build_presentation():
    presentation = Presentation()
    presentation.slide_width = Inches(13.333)
    presentation.slide_height = Inches(7.5)
    presentation.core_properties.title = "Smart Shower Water Conservation System"
    presentation.core_properties.subject = "Competition prototype presentation"
    presentation.core_properties.author = "Smart Shower Water Conservation System"

    add_title_slide(presentation)
    add_problem_slide(presentation)
    add_prototype_slide(presentation)
    add_algorithm_slide(presentation)
    add_feedback_slide(presentation)
    add_timeline_slide(presentation)
    add_technical_slide(presentation)
    add_close_slide(presentation)
    presentation.save(OUTPUT_FILE)


if __name__ == "__main__":
    build_presentation()
    print(OUTPUT_FILE)