#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include <string>


const double SPEED_MIN = 0.5;
const double SPEED_MAX = 1.2;
const int MISTAKE_MAX = 10;

struct AnyWord {
    sf::Text text;
    double speed;
    int RandomColor;

    sf::Color Any_Random_Word_Color() {
        static const std::vector random_colors = {
            sf::Color::Red,
            sf::Color::Blue,
            sf::Color::Yellow,
            sf::Color::Cyan,
            sf::Color::White,
            sf::Color::Green,
            sf::Color::Magenta
        };
        RandomColor = std::rand() % random_colors.size();
        return random_colors[RandomColor];
    }

    AnyWord(const sf::Font &font, const std::string &str, float X, float Y, float speed)
        : speed(SPEED_MIN + (std::rand() / (RAND_MAX / SPEED_MAX - SPEED_MIN))) {
        text.setFont(font);
        text.setString(str);
        text.setCharacterSize(30 + std::rand() % 40);
        text.setFillColor(Any_Random_Word_Color());
        text.setPosition(X, Y);
    }

    void Updating() {
        text.move(speed, 0);
    }
};

std::vector<std::string> FileWords(const std::string &filename) {
    std::vector<std::string> words;
    std::ifstream file(filename);
    std::string word;
    while (file >> word) {
        words.push_back(word);
    }
    return words;
}

std::vector<std::string> BigVectorWords;
std::string generateRandomWord() {
    int RandomWord = std::rand() % BigVectorWords.size();
    return BigVectorWords[RandomWord];
}

int main() {
    using namespace std;
    using namespace sf;

    BigVectorWords = FileWords("words.txt");

    Font font1, font2, font3;
    font1.loadFromFile("LittleLordFontleroyNF.TTF");
    font2.loadFromFile("ARIAL.TTF");
    font3.loadFromFile("Beyond Wonderland.TTF");

    Text instruction("Wybierz czcionke", font1, 35);
    instruction.setFillColor(Color::Red);
    instruction.setPosition(110, 40);

    Text text1("START", font1, 30);
    text1.setFillColor(Color::Magenta);
    text1.setPosition(150, 100);

    Text text2("START", font2, 30);
    text2.setFillColor(Color::Magenta);
    text2.setPosition(150, 200);

    Text text3("START", font3, 30);
    text3.setFillColor(Color::Magenta);
    text3.setPosition(150, 300);

    int ThisFont = -1;

    RenderWindow startWindow(VideoMode(400, 400), "Start");
    while (startWindow.isOpen()) {
        Event event;
        while (startWindow.pollEvent(event)) {
            if (event.type == Event::Closed) {
                startWindow.close();
            }
            if (event.type == Event::MouseButtonPressed) {
                if (event.mouseButton.button == Mouse::Left) {
                    if (text1.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        ThisFont = 0;
                        startWindow.close();
                    } else if (text2.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        ThisFont = 1;
                        startWindow.close();
                    } else if (text3.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        ThisFont = 2;
                        startWindow.close();
                    }
                }
            }
        }
        startWindow.clear(Color::Black);
        startWindow.draw(instruction);
        startWindow.draw(text1);
        startWindow.draw(text2);
        startWindow.draw(text3);
        startWindow.display();
    }



    RenderWindow window(VideoMode(800, 700), "Adrian's Game");
    window.setFramerateLimit(50);

    vector fonts{font1, font2, font3};

    vector<AnyWord> words;
    string currentInput;
    Text inputText;
    inputText.setFont(fonts[ThisFont]);
    inputText.setCharacterSize(50);
    inputText.setFillColor(Color::Green);
    inputText.setPosition(10, window.getSize().y - inputText.getCharacterSize() - 10);

    int missedWords = 0;
    int score = 0;
    bool przegrana = false;

    Clock clock;
    Time spawnTime = seconds(1);
    Time elapsed = clock.getElapsedTime();

    Text scoreText;
    scoreText.setFont(fonts[ThisFont]);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(10, 10);
    scoreText.setString("Punkty: " + to_string(score));

    Text LifeText;
    LifeText.setFont(fonts[ThisFont]);
    LifeText.setCharacterSize(20);
    LifeText.setFillColor(Color::Red);
    LifeText.setPosition(window.getSize().x - 200, 10);
    LifeText.setString("Twoje zycia: " + to_string(MISTAKE_MAX - missedWords));

    Texture backgroundTexture;
    backgroundTexture.loadFromFile("tlo.jpg");

    Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);

    Texture gameOverBackgroundTexture;
    gameOverBackgroundTexture.loadFromFile("GameOverBlood.jpg");

    Sprite gameOverBackgroundSprite;
    gameOverBackgroundSprite.setTexture(gameOverBackgroundTexture);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }

            if (event.type == Event::TextEntered) {
                if (event.text.unicode == '\b') {
                    if (!currentInput.empty()) {
                        currentInput.pop_back();
                    }
                } else if (event.text.unicode == '\r' || event.text.unicode == '\n') {
                    currentInput.clear();
                } else if (event.text.unicode < 128) {
                    currentInput += static_cast<char>(event.text.unicode);
                }
                inputText.setString(currentInput);
            }
        }

        if (clock.getElapsedTime() - elapsed >= spawnTime) {
            elapsed = clock.getElapsedTime();
            string word = generateRandomWord();
            words.emplace_back(fonts[ThisFont], word, 0, rand() % (window.getSize().y - 100), SPEED_MIN);
        }


        auto it = words.begin();
        while (it != words.end()) {
            if (currentInput == it->text.getString()) {
                it = words.erase(it);
                currentInput.clear();
                inputText.setString(currentInput);
                score +=10;
                scoreText.setString("Punkty: " + to_string(score));
            } else if (it->text.getPosition().x > window.getSize().x) {
                it = words.erase(it);
                missedWords++;
                if (missedWords >= MISTAKE_MAX) {
                    cout << "Game Over!\n";
                    przegrana = true;
                    window.close();
                } else {
                    LifeText.setString("Zycia " + to_string(MISTAKE_MAX - missedWords));
                }
            } else {
                ++it;
            }
        }

        for (auto &word: words) {
            word.Updating();
        }

        window.clear(Color::Black);
        window.draw(backgroundSprite);
        for (const auto &word: words) {
            window.draw(word.text);
        }
        window.draw(inputText);
        window.draw(LifeText);
        window.draw(scoreText);
        window.display();
    }

    if (przegrana) {
        RenderWindow gameoverwindow(VideoMode(500, 500), "Game Over!");
        Text gameovertekst("Game Over!", fonts[ThisFont], 50);
        gameovertekst.setFillColor(Color::Red);
        gameovertekst.setPosition(120, 210);

        Text finalScoreText;
        finalScoreText.setFont(fonts[ThisFont]);
        finalScoreText.setCharacterSize(30);
        finalScoreText.setFillColor(Color::White);
        finalScoreText.setPosition(170, 320);
        finalScoreText.setString("Wynik: " + to_string(score));

        while (gameoverwindow.isOpen()) {
            Event event;
            while (gameoverwindow.pollEvent(event)) {
                if (event.type == Event::Closed) {
                    gameoverwindow.close();
                }
            }
            gameoverwindow.clear(Color::Black);
            gameoverwindow.draw(gameOverBackgroundSprite);
            gameoverwindow.draw(gameovertekst);
            gameoverwindow.draw(finalScoreText);
            gameoverwindow.display();
        }
    }
}















