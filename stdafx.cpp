// stdafx.cpp : ǥ�� ���� ���ϸ� ��� �ִ� �ҽ� �����Դϴ�.
// LabProject.pch�� �̸� �����ϵ� ����� �˴ϴ�.
// stdafx.obj���� �̸� �����ϵ� ���� ������ ���Ե˴ϴ�.

#include "stdafx.h"

// TODO: �ʿ��� �߰� �����
// �� ������ �ƴ� STDAFX.H���� �����մϴ�.

XMFLOAT3 RollerCoasterPos(float T) {
    if (T < 1.42f) {
        float t = T - 0.71f;
        return XMFLOAT3(-10.0f, 10.0f * (8.0f * (powf(t, 4) - powf(t, 2)) + 2.0f), 10.0f * t);
    }
    else if (T < 2.21f) {
        float t = T - 0.71f;
        return XMFLOAT3(-10.0f, 0.0f, 10.0f * t);
    }
    else if (T < 5.35f) {
        float t = T - 2.21f;
        return XMFLOAT3(-10.0f * cosf(t), 0.0f, 10.0f * (sinf(t) + 1.5f));
    }
    else if (T < 6.85f) {
        float t = T - 6.85f;
        return XMFLOAT3(10.0f, 0.0f, -10.0f * t);
    }
    else if (T < 13.14f) {
        float t = T - 2.14f;
        return XMFLOAT3(10.0f * (1.0f - 0.1f * (t - 3.0f * XM_PI / 2.0f)), 10.0f * (sinf(t) + 1.0f), -10.0f * cosf(t));
    }
    else {
        float t = T - 13.14f;
        return XMFLOAT3(10.0f * (1.0f - 0.2f * XM_PI), 0.0f, -10.0f * t);
    }
}
int Scene_number = 1;