#include <SFML/Graphics.hpp>
#include<cmath>
// TODO: GUI with option to tweak the pendulum on the go
// TODO: add dotted path (streaklines) for the bobs
struct pendulum{
	float m = 20; //mass
	float r = 200; //length
	float o[2] = {500,300}; //origin of the pendulum (not its bob)
	float a = -M_PI/2; // angle wrt vertical (in rad)
	float a_v = 0; // angular velocity (in rad/s)
	float a_a = 0; // angular acceleration (in rad/s2)
	float w = 1; // rod width
};

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
	bool ispaused = false;

	float g = 0.01;
	float a_a[2];
	struct pendulum p1,p2;
	p1.m = 10;
	p2.m = 40;
	p1.r = 200;
	p2.r = 200;
//	sf::Clock clock;
//	float t_init = clock.getElapsedTime().asMilliseconds(); // inital time
//	float dt = 0; // initially time elapsed = 0

    	sf::RenderWindow window(sf::VideoMode(600, 600), "Double Pendulum!");

    	sf::RectangleShape rod1(sf::Vector2f(p1.r, p1.w)); // rod shape for pendulum 1
    	sf::RectangleShape rod2(sf::Vector2f(p2.r, p2.w)); // rod shape for pendulum 2

    	sf::CircleShape bob1(p1.m); // bob shape (pend 1)
    	sf::CircleShape bob2(p2.m); // bob shape (pend 2)
    	bob1.setFillColor(sf::Color::White);
    	bob2.setFillColor(sf::Color::White);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
	    if (event.type == sf::Event::LostFocus)
		        ispaused = true;
	    if (event.type == sf::Event::GainedFocus)
		        ispaused = false;
        }
	
	if(!ispaused){
	
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
	window.draw(rod1); 
	window.draw(rod2); 
        window.draw(bob1);
        window.draw(bob2);
        window.display();
    }
}
