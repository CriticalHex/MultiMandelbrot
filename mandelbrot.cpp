#include<iostream>
#include<math.h>
#include<SFML\Graphics.hpp>
#include<complex>
#include<thread>
#include<vector>
#include<list>

using namespace std;

int iterations(complex<double> c) {
	complex<double> z(0, 0);
	int count = 0;
	while (abs(z) < 2 && count < 255) {
		z = (pow(z, 2)) + c;
		count += 1;
	}
	return count;
}


void fill_array(sf::VertexArray& set, float scale, float width, float height, sf::Vector2f mouse, sf::Vector2f &last_shift, long double &last_scale, int thread, int max_threads) {
	sf::Vector2f shift;
	long double horizontalStart;
	long double horizontalEnd;
	long double verticalStart;
	long double verticalEnd;
	sf::Vector2f origin((width / 2), (height / 2));
	long double scaleStart = (-2 / scale);
	long double scaleEnd = (2 / scale);
	long double horizontalSize = (width / (abs(scaleStart) + abs(scaleEnd)));
	long double verticalSize = (height / (abs(scaleStart) + abs(scaleEnd)));
	long double horizontalRes = (abs(scaleStart) + abs(scaleEnd)) / width * 1;
	long double verticalRes = (abs(scaleStart) + abs(scaleEnd)) / height * 1;
	cout << scale << endl;

	shift = origin + ((origin - mouse) * scale);

	horizontalStart = scaleStart + ((origin.x - shift.x) / horizontalSize);
	horizontalEnd = scaleEnd + ((origin.x - shift.x) / horizontalSize);
	verticalStart = scaleStart + ((origin.y - shift.y) / verticalSize);
	verticalEnd = scaleEnd + ((origin.y - shift.y) / verticalSize);

	sf::Vector2f position;
	int num;
	for (long double t = horizontalStart; t < horizontalEnd; t += horizontalRes) {

		for (long double m = verticalStart; m < verticalEnd; m += verticalRes) {

			position = sf::Vector2f(t * horizontalSize, m * verticalSize) + shift;

			if (position.x >= 0 and position.x <= width and position.y >= 0 and position.y <= height) {

				if (position.x >= ((width / max_threads) * thread) and position.x <= ((width / max_threads) * (thread + 1))) {

					complex<double> c(t, m);

					num = iterations(c);

					set.append(sf::Vertex(position, sf::Color(num * 8, num * 6, num * 12)));
				}
			}
		}
	}
}

void make_set(list<sf::VertexArray>& sets, list<sf::VertexArray>::iterator setIter, list<thread>& active_threads, int max_threads, long double scale, int window_width, int window_height, sf::Vector2f mouse, sf::Vector2f& last_shift, long double& last_scale) {
	int i = 0;
	for (setIter = sets.begin(); setIter != sets.end(); setIter++) {
		(*setIter).clear();
	}
	for (auto& s : sets) {
		active_threads.emplace_back(fill_array, ref(s), scale, window_width, window_height, mouse, ref(last_shift), ref(last_scale), i, max_threads);
		i++;
	}
}

int main() {
	//RENDER SETUP----------------------------------------------------------
	sf::RenderWindow window(sf::VideoMode(1000, 1000), "Mandelbrot Set", sf::Style::None);
	window.setPosition(sf::Vector2i(460, 20));
	window.setFramerateLimit(60);

	//VARIABLES-------------------------------------------------------------
	sf::Color bgColor = sf::Color(8, 6, 12);
	long double scale = 1;
	sf::Vector2f last_shift;
	long double last_scale = 0;
	sf::Event event;
	list<thread> active_threads;
	int max_threads = 12;
	list<sf::VertexArray> sets;
	list<sf::VertexArray>::iterator setIter;
	sf::CircleShape dot(1);
	dot.setFillColor(sf::Color::Green);
	dot.setPosition(500, 500);
	int totalPoints = 0;

	//INITIAL SET CREATION---------------------------------------------------
	for (int i = 0; i < max_threads; i++) {
		sf::VertexArray s1;
		s1.setPrimitiveType(sf::PrimitiveType::Points);
		sets.push_back(s1);
	}
	make_set(sets, setIter, active_threads, max_threads, scale, window.getSize().x, window.getSize().y, sf::Vector2f(500,500), last_shift, last_scale);

	//GAME LOOP--------------------------------------------------------------
	while (window.isOpen()) {
		while (window.pollEvent(event))
		{
			// Close window: exit--------------------------------------------------------------------------------------------------------------------------
			if (event.type == sf::Event::Closed)
				window.close();

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
				window.close();
			}

			if (event.type == sf::Event::MouseWheelScrolled) {
				if (event.mouseWheelScroll.delta > 0) {
					scale *= 2;
				}
				else if (event.mouseWheelScroll.delta < 0) {
					scale /= 2;
				}
				make_set(sets, setIter, active_threads, max_threads, scale, window.getSize().x, window.getSize().y, sf::Vector2f(event.mouseWheelScroll.x, event.mouseWheelScroll.y), last_shift, last_scale);
				//cout << event.mouseWheelScroll.x << ", " << event.mouseWheelScroll.y << endl;
				//60.54, 495.55
				//event.mouseWheelScroll.x, event.mouseWheelScroll.y
			}
		}
		for (auto& th : active_threads) {
			if (th.joinable())
				th.join();
		}
		//RENDER--------------------------------------------------------------
		window.clear(bgColor);
		for (setIter = sets.begin(); setIter != sets.end(); setIter++) {
			totalPoints += (*setIter).getVertexCount();
			window.draw(*setIter);
		}
		//cout << totalPoints << endl;
		totalPoints = 0;
		window.draw(dot);
		window.display();
	}
	for (auto& th : active_threads) {
		if (th.joinable())
			th.join();
	}
}