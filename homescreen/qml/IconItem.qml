import QtQuick 2.0

Item {
    id: main
    Image {
        id: item; parent: loc
        x: main.x + 5; y: main.y + 5
        width: main.width - 10; height: main.height - 10;
        source: './images/HMI_AppLauncher_%1_%2-01.png'.arg(model.icon).arg(loc.pressed && (loc.index === model.index || loc.currentId === model.id) ? 'Active' : 'Inactive')
        antialiasing: item.state !== ''
        Behavior on x { enabled: item.state !== 'active'; NumberAnimation { duration: 400; easing.type: Easing.OutCubic } }
        Behavior on y { enabled: item.state !== 'active'; NumberAnimation { duration: 400; easing.type: Easing.OutCubic } }
        SequentialAnimation on rotation {
            NumberAnimation { to:  5; duration: 100 }
            NumberAnimation { to: -5; duration: 200 }
            NumberAnimation { to:  0; duration: 100 }
            running: loc.currentId !== '' && item.state !== 'active'
            loops: Animation.Infinite; alwaysRunToEnd: true
        }
        states: [
            State {
                name: 'active'
                when: loc.currentId == model.id
                PropertyChanges { target: item; x: loc.mouseX - width/2; y: loc.mouseY - height/2; scale: 1.15; z: 10 }
            },
            State {
                when: loc.currentId !== ''
                PropertyChanges {
                    target: item
                    scale: 0.85
                    opacity: 0.75
                }
            }
        ]
        transitions: Transition { NumberAnimation { properties: 'scale, opacity, x, y'; duration: 150; easing.type: Easing.OutCubic} }
    }
}
