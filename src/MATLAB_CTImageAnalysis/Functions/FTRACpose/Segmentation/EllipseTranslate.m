function [ellipseTranslated] = EllipseTranslate(el, x, y)
    ratio = el(1) * x * x + el(2) * x * y + el(3) * y * y + el(4) * x + el(5) * y + el(6);
    ellipseTranslated(1) = el(1) / -ratio;
    ellipseTranslated(2) = el(2) / -ratio;
    ellipseTranslated(3) = el(3) / -ratio;
    ellipseTranslated(4) = (2 * el(1) * x + el(2) * y + el(4)) / - ratio;
    ellipseTranslated(5) = (el(2) * x + 2 * el(3) * y + el(5)) / - ratio;
    ellipseTranslated(6) = -1;

