const hotKeyButton = document.querySelector('#hot-key-b');
const touchPadButton = document.querySelector('#touch-pad-b');

const touchPad = document.querySelector('.touch-pad');
const hotKeys = document.querySelector('#hot-keys')

const pointer = document.querySelector('.circle');
const debug = document.querySelector('#debug');
const debugButton = document.querySelector('#debug-b');

debugButton.onclick = () => {
    debug.classList.remove('not-active');
}
debug.onclick = () => {
    debug.classList.add('not-active');
}

hotKeyButton.onclick = () => {
    hotKeyButton.classList.add('selected');
    touchPadButton.classList.remove('selected')
    hotKeys.classList.remove('not-active');
    touchPad.classList.add('not-active');
}
touchPadButton.onclick = () => {
    hotKeyButton.classList.remove('selected');
    touchPadButton.classList.add('selected')
    touchPad.classList.remove('not-active');
    hotKeys.classList.add('not-active');
}
const prevDirection = {
    point: {
        x: 0,
        y: 0
    },
    time: null
}
const socket = new WebSocket("ws://" + window.location.hostname + ":228")
socket.binaryType = "arraybuffer"

function sendRaw() {
    const arr = Array.prototype.slice.call(arguments, 1)
    const buffer = new Uint8Array(arr.length);
    for (let i = 0; i < arr.length; i++) {
        buffer[i] = arr[i]
    }
    socket.send(buffer)
}

function sendLBMClick() {
    sendRaw(3)
}

function sendMousePos(x, y) {
    if (x > 255)
        x = 255
    if (y > 255)
        y = 255
    console.log('xy', x, y)
    const buffer = new ArrayBuffer(9);
    const view = new DataView(buffer);
    view.setUint8(0, 0);
    view.setFloat32(1, x, false);
    view.setFloat32(5, y, false);
    socket.send(buffer)
}

const usePosition = (event) => {
    const rect = event.target.getBoundingClientRect();

    const touch = event.targetTouches[0];
    const x = touch.clientX - rect.left - pointer.clientWidth / 2;
    const y = touch.clientY - rect.top - pointer.clientHeight / 2;
    return { rect, x, y, touch };
}
let lastClick = 0;
touchPad.addEventListener('touchstart', (event) => {
    event.preventDefault();
    pointer.classList.remove('not-active')
    const { x, y } = usePosition(event);

    pointer.style.left = x + 'px';
    pointer.style.top = y + 'px';

    //let time = Date.now();
    //const time_between_taps = 200; // 200ms
    // if (time - lastClick < time_between_taps) {
    //     // do stuff
    //     sendLBMClick(true);
    //     debug.innerHTML = 'gobvovodfofgofodsfoksdkfdsfsfksdlkfsldkf'
    //     console.log('double click')
    // } else {
    //     sendLBMClick(false);
    // }
    // sendLBMClick();
    //lastClick = time;
});
touchPad.addEventListener('touchend', () => pointer.classList.add('not-active'));
touchPad.addEventListener('touchmove', (event) => {
    const { x, y, rect, touch } = usePosition(event);

    const delta = {
        x: touch.pageX - prevDirection.point.x,
        y: touch.pageY - prevDirection.point.y,
        time: Date.now() - prevDirection.time
    }
    const direction = {
        x: delta.x > 0 ? 1 : -1,
        y: delta.y > 0 ? 1 : -1
    }
    const speed = {
        x: (Math.abs(delta.x) / delta.time) * 10,
        y: (Math.abs(delta.y) / delta.time) * 10
    }
    debug.innerHTML = speed.x + +speed.y
    if (x >= 0 && x < rect.width - pointer.clientWidth)
        pointer.style.left = x + 'px';
    if (y >= 0 && y < rect.height - pointer.clientHeight)
        pointer.style.top = y + 'px';

    sendMousePos(direction.x * speed.x, direction.y * speed.y)
        //console.log(direction.x * speed.x, direction.y * speed.y)
    prevDirection.point.x = touch.pageX;
    prevDirection.point.y = touch.pageY;
    prevDirection.time = Date.now();

});
// Example POST method implementation:
// async function postData(url = '', data = {}) {
// 	// Default options are marked with *
// 	const response = await fetch(url, {
// 		method: 'POST', // *GET, POST, PUT, DELETE, etc.
// 		mode: 'cors', // no-cors, *cors, same-origin
// 		cache: 'no-cache', // *default, no-cache, reload, force-cache, only-if-cached
// 		credentials: 'same-origin', // include, *same-origin, omit
// 		headers: {
// 		'Content-Type': 'application/json'
// 		// 'Content-Type': 'application/x-www-form-urlencoded',
// 		},
// 		redirect: 'follow', // manual, *follow, error
// 		referrerPolicy: 'no-referrer', // no-referrer, *no-referrer-when-downgrade, origin, origin-when-cross-origin, same-origin, strict-origin, strict-origin-when-cross-origin, unsafe-url
// 		body: JSON.stringify(data) // body data type must match "Content-Type" header
// 	});
// 	return response.json(); // parses JSON response into native JavaScript objects
// }