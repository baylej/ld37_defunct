/*
	Copyright 2016 Jonathan Bayle, Thomas Medioni

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

#include <vivace/vivace.hpp>

using namespace vivace;

#include <exception>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

#include <glm/glm.hpp>

using namespace glm;

class Game: public Object {
public:
	Game():
		dbg_font(al_create_builtin_font())
	{
		bg_colour   = al_color_hsv(0., .8, .2);
		char_colour = al_color_hsv(200., .8, .2);
	}

	virtual void update()
	{
		double this_update = al_get_time();
		delta_t = this_update - last_update;
		if (delta_t >= 1.) {
			cerr << "lag!" << endl;
		}
		else {
			pos.x += speed.x * 50. * delta_t;
			pos.y += speed.y * 50. * delta_t;
		}
		last_update = this_update;

		sum_t += delta_t;
		if (sum_t >= 1.)
		{
			sum_t = 0;
			mk_fps_string();
		}
	};

	virtual void draw()
	{
		al_clear_to_color(bg_colour);
		al_draw_filled_rectangle(380+pos.x, 280-pos.y, 420+pos.x, 320-pos.y, char_colour);

		al_draw_text(dbg_font.get(), al_map_rgb_f(1,1,1), 792, 8, ALLEGRO_ALIGN_RIGHT, fps_string.c_str());
		al_flip_display();
	};

	virtual void handle(const ALLEGRO_EVENT& event)
	{
		float vecval = 0.;
		switch (event.type) {
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
			cerr << "Bye!" << endl;
			throw 1;

		// Handle Keyboard events
		case ALLEGRO_EVENT_KEY_DOWN:
			vecval = 1.;
		case ALLEGRO_EVENT_KEY_UP:
			switch (event.keyboard.keycode) {
			case ALLEGRO_KEY_UP:
				speed.y = vecval;
				break;
			case ALLEGRO_KEY_DOWN:
				speed.y = -vecval;
				break;
			case ALLEGRO_KEY_LEFT:
				speed.x = -vecval;
				break;
			case ALLEGRO_KEY_RIGHT:
				speed.x = vecval;
				break;
			}
			// normalize(vecX({0.})) is undefined!
			if (speed != vec2(0., 0.)) {
				// speed is 1 for all directions
				speed = normalize(speed);
			}
			break;
		default:
			cerr << "Unknown event type " << event.type << endl;
		}
	};

private:
	double last_update = 0.;
	double delta_t = 0.;
	double sum_t = 0.;
	string fps_string;
	ALLEGRO_COLOR bg_colour;
	ALLEGRO_COLOR char_colour;
	vec2 pos{0., 0.}; // position in meter
	vec2 speed{0., 0.}; // in meter per seconds
	unique_ptr<ALLEGRO_FONT, al_font_deleter> dbg_font; // TODO move to global scope

	void mk_fps_string()
	{
		ostringstream oss;
		oss << static_cast<int>(1/delta_t) << " fps";
		fps_string = oss.str();
	};
};

int main(void) {
	try {
		Vivace engine("GAME_NAME"s, ""s);
		unique_ptr<ALLEGRO_DISPLAY, al_display_deleter> dsp(al_create_display(800, 600));
		Game game;
		Basic_loop main_loop(1/30., game);
		main_loop.register_event_source(al_get_display_event_source(dsp.get()));
		main_loop.register_event_source(al_get_keyboard_event_source());
		main_loop.run();
	}
	catch (std::exception ex) {
		std::cerr << ex.what() << std::endl;
	}
	catch (int ex) {}
}
