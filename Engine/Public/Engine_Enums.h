#pragma once

enum TextureType
{
    TextureType_NONE = 0,
    TextureType_DIFFUSE = 1,
    TextureType_SPECULAR = 2,
    TextureType_AMBIENT = 3,
    TextureType_EMISSIVE = 4,
    TextureType_HEIGHT = 5,
    TextureType_NORMALS = 6,
    TextureType_SHININESS = 7,
    TextureType_OPACITY = 8,
    TextureType_DISPLACEMENT = 9,
    TextureType_LIGHTMAP = 10,
    TextureType_REFLECTION = 11,
    TextureType_BASE_COLOR = 12,
    TextureType_NORMAL_CAMERA = 13,
    TextureType_EMISSION_COLOR = 14,
    TextureType_METALNESS = 15,
    TextureType_ROUGHNESS = 16,
    TextureType_AMBIENT_OCCLUSION = 17,
    TextureType_MRM = 18,
    TextureType_SHEEN = 19,
    TextureType_MRAO = 20,
    TextureType_ORM = 21,
    TextureType_RMA = 22,
    TextureType_MASK = 23,
    TextureType_FRENSEL_NORMAL = 24,
};

enum CollisionType
{
    TRIGGER,
    COLLISION,
};

enum CinemaEventType
{
    CHANGE_ANIMATION,
    PLAY_SOUND,
    STOP_SOUND,
    ACTIVE_EFFECT,
    INACITVE_EFFECT,
    ETC,
    CINEMA_EVENT_END,
};




