const hotKeyButton = document.querySelector('#hot-key-b');
const touchPadButton = document.querySelector('#touch-pad-b');

const touchPad = document.querySelector('.touch-pad');
const hotKeys = document.querySelector('#hot-keys')

const pointer = document.querySelector('.circle');
hotKeyButton.onclick = () => {
    hotKeys.classList.remove('not-active');
    touchPad.classList.add('not-active');
}
touchPadButton.onclick = () => {
    touchPad.classList.remove('not-active');
    hotKeys.classList.add('not-active');
}
const getPosition = (event) => {
    const rect = event.target.getBoundingClientRect();
    const x = event.targetTouches[0].clientX - rect.left - pointer.clientWidth / 2; 
    const y = event.targetTouches[0].clientY - rect.top - pointer.clientHeight / 2;  
    return { rect, x, y };
}
touchPad.addEventListener('touchstart', (event) => {
    pointer.classList.remove('not-active') 
    const { x, y } = getPosition(event);

    pointer.style.left = x + 'px';
    pointer.style.top = y + 'px';
});
touchPad.addEventListener('touchend', () => pointer.classList.add('not-active'));
touchPad.addEventListener('touchmove', (event) => {
    const { x, y, rect } = getPosition(event); 
    if (x >= 0 && x < rect.width - pointer.clientWidth)
        pointer.style.left = x + 'px';
    if (y >= 0 && y < rect.height - pointer.clientHeight)
        pointer.style.top = y + 'px';
});
