#******************************************************************************
#  Copyright 2023 Pierre Bayerl and Michael Thoma
#  This program and the accompanying materials are made available under the
#  terms of the MIT License, which is available in the project root.
#******************************************************************************
from fruits.Apple import Apple, createApple
from fruits.Basket import Basket
from fruits.Color import Color
from pytest import raises

def test_basic_apple():
    apple = Apple.from_json('{"color": "green", "size_cm": 1.2}')
    assert apple.color == Color.green
    assert apple.size_cm == 1.2

    apple = Apple.from_json('{"color": "yellow", "size_cm": 1.3}')
    assert apple.color == Color.yellow
    assert apple.size_cm == 1.3

    apple = Apple(Color.green,1.4);
    assert apple.color == Color.green
    assert apple.size_cm == 1.4

    apple = Apple(Color.green, 1.0);
    assert apple.color == Color.green
    assert apple.size_cm == 1.0

def test_basic_basket():
    basket = Basket.from_json('''
        {
            "apples": [
                {"color": "green", "size_cm": 3.1},
                {"color": "red", "size_cm": 1.2},
                {"color": "yellow", "size_cm": 3.3}
            ]
        }
    ''')

    assert(len(basket.apples) == 3);
    assert(basket.apples[0].color == Color.green);
    assert(basket.apples[1].color == Color.red);
    assert(basket.apples[2].color == Color.yellow);
    assert(basket.apples[0].size_cm == 3.1);
    assert(basket.apples[1].size_cm == 1.2);
    assert(basket.apples[2].size_cm == 3.3);

def test_basic_bad_basket_undefined_field():
    with raises(Exception, match=r'.*Received undefined initialization arguments.*'):
        _ = Basket.from_json('''
            {
                "xx_apples_wrong_name": [
                    {"color": "green", "size_cm": 3.1},
                    {"color": "red"},
                    {"color": "yellow", "size_cm": 3.3}
                ]
            }
        ''')
    with raises(Exception, match=r'.*Received undefined initialization arguments.*'):
        _ = Basket.from_json('''
            {
                "apples": [
                    {"color": "green", "size_cm": 3.1},
                    {"ERROR": "red"},
                    {"color": "yellow", "size_cm": 3.3}
                ]
            }
        ''')

def test_basic_bad_basket_missing_required_field():
    with raises(KeyError, match='color'):
        _ = Basket.from_json('''
            {
                "apples": [
                    {"color": "green", "size_cm": 3.1},
                    {"size_cm": 3.3}
                ]
            }
        ''')
