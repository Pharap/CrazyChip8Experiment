#include <SDL.h>

#include "chip8.h"

#include "sdl_shared.h"
#include "sdl_display.h"
#include "sdl_keyboard.h"

std::vector<chip8::byte> create_program_b()
{
	using namespace chip8::lang;

	label label_a;

	return
		program(),
		reg_0 = 0x00,
		reg_1 = 0x00,
		reg_2 = 0x00,

		label_a,

		load_sprite(reg_0),
		draw_sprite(reg_1, reg_2, 5),

		reg_0 += 1,
		reg_1 += 5,

		reg_3 = 60,
		skip_if(reg_1 != reg_3),
			reg_2 += 9,

		skip_if(reg_1 != reg_3),
			reg_1 = 0,

		reg_3 = 0x10,
		skip_if(reg_0 == reg_3),
			jump(label_a),

		end_program;

	/*return
		program(),
		reg_0 = 0x10,
		reg_1 = 0x01,

		label_a,

		reg_0 -= reg_1,
		load_sprite(reg_0),

		reg_3 = 0x01,
		reg_4 = 0x01,
		draw_sprite(reg_3, reg_4, 5),

		reg_2 = 0x02,

		label_b,
		skip_if(key_pressed(reg_2)),
			jump(label_b),

		skip_if(reg_0 != 0x00),
			jump(label_a),

		end_program;*/
}

std::vector<chip8::byte> create_program_a()
{
	using chip8::register_id;

	chip8::vector_byte_writer writer;
	chip8::encoder encoder(writer);

	encoder.encode_load(register_id::reg_0, 0x01);
	encoder.encode_load(register_id::reg_1, 0x00);
	encoder.encode_load(register_id::reg_2, 0x00);

	encoder.encode_clear_screen();
	encoder.encode_load_digit_sprite(register_id::reg_0);
	encoder.encode_draw(register_id::reg_1, register_id::reg_2, 5);

	encoder.encode_exit();

	return std::move(writer.get_container());
}

chip8::byte program_a[64];

int main(int argument_count, char * arguments[])
{
	auto window = sdl_create_window("SDL!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 400, SDL_WindowFlags::SDL_WINDOW_ALLOW_HIGHDPI);
	auto renderer = sdl_create_renderer(window, -1, SDL_RendererFlags::SDL_RENDERER_ACCELERATED | SDL_RendererFlags::SDL_RENDERER_PRESENTVSYNC);
	
	auto display = std::make_shared<sdl_display>(renderer, 0, 0, 8, 8);
	auto keyboard = std::make_shared<sdl_keyboard>();

	chip8::processor processor(std::static_pointer_cast<chip8::display>(display), std::static_pointer_cast<chip8::keyboard>(keyboard));

	processor.load_default_sprite_rom();

	//const auto program_a = create_program_a();

	const auto program_b = std::move(create_program_b());

	processor.load_program(std::begin(program_b), std::end(program_b));

	processor.start();

	bool running = true;
	while(running)
	{
		SDL_Event e;
		while(SDL_PollEvent(&e) != 0)
		{
			switch(e.type)
			{
			case SDL_EventType::SDL_QUIT:
				running = false;
				break;
			}
		}

		//processor.reset();
		processor.run(64);

		//SDL_RenderClear(renderer.get());
		processor.update_display();
	}

	return EXIT_SUCCESS;
}