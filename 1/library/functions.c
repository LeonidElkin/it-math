double f(double x, double y) {
	return 0.0;
}

double g(double x, double y) {
	if (y == 0) return 100 - 200 * x;
	if (x == 0) return 100 - 200 * y;
	if (y == 1) return 200 * x - 100;
	if (x == 1) return 200 * y - 100;
}
