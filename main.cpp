#include <SFML/Graphics.hpp>
#include<iostream>
#include<cmath>
#include<time.h>

#define N 100

// TODO: add dotted path (streaklines) for the bobs
struct pendulum{
	float m = 20; //mass
	float r = 200; //length
	float o[2] = {500,300}; //origin of the pendulum (not its bob)
	float a = -M_PI/2; // angle wrt vertical (in rad)
	float a_v = 0; // angular velocity (in rad/s)
	float a_a = 0; // angular acceleration (in rad/s2)
	float w = 2; // rod width
};

bool isTextFieldClicked(sf::Text& textField, sf::Vector2f& coords){
	return textField.getGlobalBounds().contains(coords);
}

//keypress 'f' to change the second pendulum's angle
void changePendulumAngle(pendulum* a, pendulum* b){
	int den = rand()%10 + 1;
	a->a = 2*M_PI/den;
	den = rand()%10 + 1;
	b->a = 2*M_PI/den;
	a->a_v = 0;
	a->a_a = 0;
	b->a_v = 0;
	b->a_a = 0;
}

void resetTextFields(sf::Text& L1, sf::Text& L2, sf::Text& m1, sf::Text& m2, pendulum* a1, pendulum* a2){
	L1.setString("pendulum #1 length: " + std::to_string(int(a1->r)));
	L2.setString("pendulum #2 length: " + std::to_string(int(a2->r)));
	m1.setString("pendulum #1 mass: " + std::to_string(int(a1->m)));
	m2.setString("pendulum #2 mass: " + std::to_string(int(a2->m)));
}

void changeShape(sf::RectangleShape& rod1, sf::RectangleShape& rod2, sf::CircleShape& bob1, sf::CircleShape& bob2, pendulum* a1, pendulum* a2){
	rod1.setSize(sf::Vector2f(a1->r, a1->w));
	rod2.setSize(sf::Vector2f(a2->r, a2->w));
	bob1.setRadius(a1->m);
	bob2.setRadius(a2->m);

	a1->a_v = 0;
	a2->a_v = 0;
	a1->a_a = 0;
	a2->a_a = 0;
}

void ang_acc(float& m1, float& m2, float& g, float& a1, float& a2, float& a1_v, float& a2_v, float& L1, float& L2, float* res){
	float num1 = -g*(2*m1 + m2)*sin(a1);
	float num2 = -m2*g*sin(a1 - 2*a2);
	float num3 = -2*sin(a1 - a2)*m2;
	float num4 = a1_v*a1_v*L2 + a1_v*a1_v*L1*cos(a1 - a2);
	float den = L1*(2*m1 + m2 - m2*cos(2*a1 - 2*a2));
	res[0] = (num1 + num2 + num3*num4)/den;

	num1 = 2*sin(a1 - a2);
	num2 = a1_v * a1_v * L1 * (m1+m2);
	num3 = g * (m1+m2) * cos(a1); 
	num4 = a2_v * a2_v * L2 * m2 * cos(a1 - a2);
	den = L2 * (2*m1 + m2 - m2*cos(2*a1 - 2*a2));

	res[1] = (num1*(num2 + num3 + num4))/den;
}

int main()
{
	srand(time(0));
	int temp_num = 10; // used for temporary int numbers one might need
	bool isPaused = false;
	sf::Vector2f currMouseCoords;

	std::string ev_num = "";
	char fieldClicked = '0';

	float g = 0.01;
	float a_a[2];
	struct pendulum p1,p2;
	p1.m = 10;
	p2.m = 10;
	p1.r = 200;
	p2.r = 200;
//	sf::Clock clock;
//	float t_init = clock.getElapsedTime().asMilliseconds(); // inital time
//	float dt = 0; // initially time elapsed = 0

    	sf::RenderWindow window(sf::VideoMode(600, 600), "Double Pendulum!");
	p1.o[0] = window.getSize().x/2;
	p1.o[1] = window.getSize().y/2;

    	sf::RectangleShape rod1(sf::Vector2f(p1.r, p1.w)); // rod shape for pendulum 1
    	sf::RectangleShape rod2(sf::Vector2f(p2.r, p2.w)); // rod shape for pendulum 2

    	sf::CircleShape bob1(p1.m); // bob shape (pend 1)
    	sf::CircleShape bob2(p2.m); // bob shape (pend 2)
    	bob1.setFillColor(sf::Color::White);
    	bob2.setFillColor(sf::Color::White);

	// LOAD FONT AND DISPLAY BANNER MESSAGE
	sf::Font font;
	if (!font.loadFromFile("data/arial.ttf"))
	{
	    // error...
	}
	sf::Text bannerMsg;
	bannerMsg.setFont(font);
	bannerMsg.setCharacterSize(48); // in pixels, not points!
	bannerMsg.setString("Double Pendulum");
	bannerMsg.setStyle(sf::Text::Bold | sf::Text::Underlined);
	bannerMsg.setPosition(window.getSize().x/2 - bannerMsg.getLocalBounds().width/2,0);

	int text_size = 16, text_spacing = 4, text_xoffset = 10;

	sf::Text helpText;
	helpText.setFont(font);
	helpText.setCharacterSize(14); // in pixels, not points!
	helpText.setString("(click on the textfields to tweak the pendulum)");
	helpText.setPosition(text_xoffset, 0);

	// p1_L, p2_L, p1_m, and p2_m are textfields for the respective values
	sf::Text p1_L;
	p1_L.setFont(font);
	p1_L.setCharacterSize(text_size); // in pixels, not points!
	p1_L.setPosition(text_xoffset, (text_size + text_spacing)*1);

	sf::Text p2_L;
	p2_L.setFont(font);
	p2_L.setCharacterSize(text_size); // in pixels, not points!
	p2_L.setPosition(text_xoffset, (text_size + text_spacing)*2);

	sf::Text p1_m;
	p1_m.setFont(font);
	p1_m.setCharacterSize(text_size); // in pixels, not points!
	p1_m.setPosition(text_xoffset, (text_size + text_spacing)*3);

	sf::Text p2_m;
	p2_m.setFont(font);
	p2_m.setCharacterSize(text_size); // in pixels, not points!
	p2_m.setPosition(text_xoffset, (text_size + text_spacing)*4);

	resetTextFields(p1_L, p2_L, p1_m, p2_m, &p1, &p2);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
	    if (event.type == sf::Event::LostFocus)
		        isPaused = true;
	    if (event.type == sf::Event::GainedFocus)
		        isPaused = false;
	    if (event.type == sf::Event::KeyPressed)
	    {
		    if(event.key.code == sf::Keyboard::F){
			    changePendulumAngle(&p1, &p2);
		    }
	    }

 	    if (event.type == sf::Event::TextEntered)
 	   	{
 	   	    if ((event.text.unicode < 58 && event.text.unicode > 47) || event.text.unicode == 8 || event.text.unicode == 13)
		    	if (fieldClicked != '0'){
				if (event.text.unicode == 8){ //backspace event
					if(ev_num.size() > 0){
						ev_num.erase(ev_num.size()-1,1);
					}
				}
				else if (event.text.unicode == 13){ // press enter event
					if (ev_num.size() > 0){
						switch(fieldClicked){
							case '1':
								temp_num = std::stoi(ev_num);
								p1.r = temp_num; // from string to num
								changeShape(rod1, rod2, bob1, bob2, &p1, &p2);
								break;
							case '2':
								temp_num = std::stoi(ev_num);
								p2.r = temp_num; 
								changeShape(rod1, rod2, bob1, bob2, &p1, &p2);
								break;
							case '3':
								temp_num = std::stoi(ev_num);
								p1.m = temp_num;
								changeShape(rod1, rod2, bob1, bob2, &p1, &p2);
								break;
							case '4':
								temp_num = std::stoi(ev_num);
								p2.m = temp_num; 
								changeShape(rod1, rod2, bob1, bob2, &p1, &p2);
								break;
							}
						ev_num = ""; //reset ev_num
						resetTextFields(p1_L, p2_L, p1_m, p2_m, &p1, &p2);
						fieldClicked = '0';
					}
				}
					
				else{
	 	   	        	ev_num += static_cast<char>(event.text.unicode);
				}
				switch(fieldClicked){
					case '1':
						p1_L.setString("pendulum #1 length: " + ev_num);
						break;
					case '2':
						p2_L.setString("pendulum #2 length: " + ev_num);
						break;
					case '3':
						p1_m.setString("pendulum #1 mass: " + ev_num);
						break;
					case '4':
						p2_m.setString("pendulum #2 mass: " + ev_num);
						break;
			    		}
 	   			}

		}
        }
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		currMouseCoords = sf::Vector2f(sf::Mouse::getPosition(window));
	//	printf("Mouse coords: (%f, %f) \n", currMouseCoords.x, currMouseCoords.y);
	//
		if(isTextFieldClicked(p1_L, currMouseCoords)){
		//	sf::FloatRect a = p1_L.getLocalBounds();
		//	printf("left = %d, top = %d, width = %d, height = %d\n", a.left, a.top, a.width, a.height);
			resetTextFields(p1_L, p2_L, p1_m, p2_m, &p1, &p2);
			p1_L.setString("pendulum #1 length: ____");
			fieldClicked = '1';
		}
		else if(isTextFieldClicked(p2_L, currMouseCoords)){
			resetTextFields(p1_L, p2_L, p1_m, p2_m, &p1, &p2);
			p2_L.setString("pendulum #2 length: ____");
			fieldClicked = '2';
		}
		else if(isTextFieldClicked(p1_m, currMouseCoords)){
			resetTextFields(p1_L, p2_L, p1_m, p2_m, &p1, &p2);
			p1_m.setString("pendulum #1 mass: ____");
			fieldClicked = '3';
		}
		else if(isTextFieldClicked(p2_m, currMouseCoords)){
			resetTextFields(p1_L, p2_L, p1_m, p2_m, &p1, &p2);
			p2_m.setString("pendulum #2 mass: ____");
			fieldClicked = '4';
		}
		else{
			// clicking elsewhere leads to shifting the origin of the pendulum
			p1.o[0] = sf::Mouse::getPosition(window).x;
			p1.o[1] = sf::Mouse::getPosition(window).y;
		}
	}


	
	if(!isPaused){
	
	//	dt = clock.getElapsedTime().asMilliseconds(); // time difference
		ang_acc(p1.m, p2.m, g, p1.a, p2.a, p1.a_v, p2.a_v, p1.r, p2.r, a_a);
	
		p1.a_a = a_a[0];
	//	p1.a_v += p1.a_a*dt;
		p1.a_v += p1.a_a;
		p1.a += p1.a_v;
		
		p2.a_a = a_a[1];
	//	p2.a_v += p2.a_a*dt;
		p2.a_v += p2.a_a;
		p2.a += p2.a_v;
	
		rod1.setPosition(p1.o[0]+p1.w/2,p1.o[1]+p1.w/2);
		rod1.setRotation(90+p1.a*180/M_PI);
		bob1.setPosition(p1.o[0] + p1.r*cos(M_PI/2+p1.a) - p1.m , p1.o[1] + p1.r*sin(M_PI/2+p1.a) - p1.m);
	
		p2.o[0] = p1.o[0] + p1.r*cos(M_PI/2+p1.a) + p1.w/2;
		p2.o[1] = p1.o[1] + p1.r*sin(M_PI/2+p1.a) +  p1.w/2;
		rod2.setPosition(p2.o[0]-p1.w/2,p2.o[1]-p1.w/2);
		rod2.setRotation(90+p2.a*180/M_PI);
		bob2.setPosition(p2.o[0] + p2.r*cos(M_PI/2+p2.a) - p2.m , p2.o[1] + p2.r*sin(M_PI/2+p2.a) - p2.m);
	}
	
        window.clear();
	window.draw(bannerMsg);
	window.draw(helpText);
	window.draw(p1_L);
	window.draw(p2_L);
	window.draw(p1_m);
	window.draw(p2_m);
	window.draw(rod1); 
	window.draw(rod2); 
        window.draw(bob1);
        window.draw(bob2);
        window.display();
    }
}
