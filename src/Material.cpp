#include "Material.h"

Material::~Material() {

    for (Texture* pTex : pTextures) {
        delete pTex;
    }
   
}
