#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

bool g_autoPlayEnabled = false;

class (BotPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto label = CCLabelBMFont::create(
            g_autoPlayEnabled ? "AutoBot: ON" : "AutoBot: OFF", 
            "bigFont.fnt"
        );
        label->setScale(0.4f);

        auto btn = CCMenuItemSpriteExtra::create(
            label,
            this,
            menu_selector(BotPauseLayer::onToggleBot)
        );

        auto menu = CCMenu::create();
        menu->addChild(btn);
        menu->setPosition({ 85.f, winSize.height - 30.f });
        this->addChild(menu);
    }

    void onToggleBot(CCObject* sender) {
        g_autoPlayEnabled = !g_autoPlayEnabled;
        auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
        auto label = static_cast<CCLabelBMFont*>(btn->getNormalImage());
        label->setString(g_autoPlayEnabled ? "AutoBot: ON" : "AutoBot: OFF");
    }
};

class (BotPlayLayer, PlayLayer) {
    struct Fields {
        bool isHolding = false;
    };

    void update(float dt) {
        PlayLayer::update(dt);

        if (!g_autoPlayEnabled || !m_player1 || m_player1->m_isDead) {
            return;
        }

        auto player = m_player1;
        CCPoint pPos = player->getPosition();
        bool shouldPress = false;
        float lookAheadDist = 95.f; 

        CCObject* obj = nullptr;
        CCARRAY_FOREACH(m_objects, obj) {
            auto gameObj = static_cast<GameObject*>(obj);
            if (!gameObj) continue;

            CCPoint oPos = gameObj->getPosition();
            float dx = oPos.x - pPos.x;

            if (dx < 0.f || dx > lookAheadDist) continue;

            float dy = std::abs(oPos.y - pPos.y);

            if (gameObj->m_objectType == GameObjectType::Hazard) {
                if (dy < 35.f) {
                    shouldPress = true;
                    break;
                }
            }

            if (!player->m_isShip && !player->m_isWave && !player->m_isUFO) {
                if (gameObj->m_objectType == GameObjectType::Solid) {
                    if (oPos.y >= pPos.y - 5.f && oPos.y <= pPos.y + 25.f && dx < 75.f) {
                        shouldPress = true;
                        break;
                    }
                }
            }
        }

        if (shouldPress && !m_fields->isHolding) {
            player->pushButton(PlayerButton::Jump);
            m_fields->isHolding = true;
        } else if (!shouldPress && m_fields->isHolding) {
            player->releaseButton(PlayerButton::Jump);
            m_fields->isHolding = false;
        }
    }
};
