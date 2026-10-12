import time
import unittest

from vision import LABELS, TomatoVision, create_test_frame, make_training_crop


class TomatoVisionTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.engine = TomatoVision()
        time.sleep(0.2)

    @classmethod
    def tearDownClass(cls):
        cls.engine.stop_event.set()
        cls.engine.thread.join(timeout=2)

    def test_seeded_model_predicts_each_synthetic_ripeness_class(self):
        for index, label in enumerate(LABELS):
            with self.subTest(label=label):
                prediction = self.engine._predict(make_training_crop(label, 77))
                self.assertEqual(prediction["label"], label)
                self.assertAlmostEqual(sum(prediction["probabilities"].values()), 1.0, places=5)

    def test_test_scene_proposes_warm_color_fruit(self):
        candidates = self.engine._find_candidates(create_test_frame(1.0))
        self.assertGreaterEqual(len(candidates), 2)
        for candidate in candidates:
            self.assertIn("box", candidate)
            self.assertIn("center", candidate)

    def test_selected_green_tomato_gets_roi_prediction(self):
        frame = create_test_frame(1.0)
        with self.engine.lock:
            self.engine.raw_frame = frame.copy()
            self.engine.width, self.engine.height = frame.shape[1], frame.shape[0]
        self.assertTrue(self.engine.set_roi([105, 220, 100, 105])["ok"])
        self.engine._process_frame(frame)
        self.assertEqual(self.engine.status()["prediction"]["label"], "green")

    def test_training_requires_multiple_examples_in_two_classes(self):
        self.assertFalse(self.engine._can_train([]))
        samples = [
            {"label": "green"}, {"label": "green"},
            {"label": "red"}, {"label": "red"},
        ]
        self.assertTrue(self.engine._can_train(samples))


if __name__ == "__main__":
    unittest.main()
