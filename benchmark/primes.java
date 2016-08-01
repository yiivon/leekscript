
class Primes {

	private static boolean is_prime(int number) {
		for (int k = 1; 36 * k * k - 12 * k < number; ++k) {
			if ((number % (6 * k + 1) == 0) || (number % (6 * k - 1) == 0)) {
				return false;
			}
		}
		return true;
	}
	
	public static void main(String[] args) {
		long t = System.nanoTime();
		int c = 2;
		for (int i = 5; i < 1000000; i += 6) {
			if (is_prime(i)) c++;
			if (is_prime(i + 2)) c++;
		}
		long e = System.nanoTime() - t;
		System.out.println(c);
		System.out.println("Time: " + (e / 1000000) + "ms");
	}
}
