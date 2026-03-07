import QtQuick 2.12
import QtQuick.Controls 2.12

/**
 * @brief 加载旋转指示器
 * 
 * 用于显示页面级或全局加载状态
 */
Item {
    id: root
    width: 40
    height: 40
    visible: loading

    property bool loading: false
    property color color: "#4f46e5"
    property int strokeWidth: 3
    property int duration: 1000

    Rectangle {
        id: spinner
        width: parent.width
        height: parent.height
        radius: width / 2
        color: "transparent"
        border.color: root.color
        border.width: root.strokeWidth

        // 使用 ShaderEffect 创建渐变效果
        layer.enabled: true
        layer.effect: ShaderEffect {
            property variant source: ShaderEffectSource { sourceItem: spinner; hideSource: true }

            fragmentShader: "
                varying highp vec2 qt_TexCoord0;
                uniform highp float qt_Opacity;
                uniform sampler2D source;
                void main() {
                    highp vec4 color = texture2D(source, qt_TexCoord0);
                    // 创建渐变透明效果
                    highp float angle = atan(qt_TexCoord0.y - 0.5, qt_TexCoord0.x - 0.5);
                    highp float gradient = smoothstep(-0.8, 0.8, sin(angle));
                    gl_FragColor = color * gradient * qt_Opacity;
                }
            "
        }

        NumberAnimation {
            target: spinner
            property: "rotation"
            from: 0
            to: 360
            duration: root.duration
            loops: Animation.Infinite
            running: root.loading
            easing.type: Easing.Linear
        }
    }
}
