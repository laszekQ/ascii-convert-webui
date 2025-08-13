window.onload = () => {
ASCIIModule().then((module) => {
    const file_input = document.getElementById("file_input");
    const canvas = document.getElementById("canvas");
    const ctx = canvas.getContext("2d");

    function updateImageAndRedraw(img) {
        const width = img.width;
        const height = img.height;

        canvas.width = width;
        canvas.height = height;
        ctx.drawImage(img, 0, 0);

        const data = ctx.getImageData(0, 0, width, height);
        const rgba_data = data.data;
        const ptr_in = module._malloc(rgba_data.length);
        module.HEAPU8.set(rgba_data, ptr_in);

        const gradient = document.getElementById("grad").value.trim();
        const encoder = new TextEncoder();
        const grad_bytes = encoder.encode(gradient + '\0');
        const grad_ptr = module._malloc(grad_bytes.length)
        module.HEAPU8.set(grad_bytes, grad_ptr);

        const grad_len = parseInt(document.getElementById("grad_len").value);
        
        const c_mode = document.querySelector("input[name='c_mode']:checked").value.charCodeAt(0);
        const mode = document.querySelector("input[name='mode']:checked").value.charCodeAt(0);

        const ptr_out = module._imgToAscii(ptr_in, width, height, grad_ptr, grad_len, c_mode, mode);

        if(!ptr_out){
            console.log('Null pointer returned!');
            return;
        }
        const text = module.UTF8ToString(ptr_out);
        document.getElementById("out_text").textContent = text;   

        module._free(ptr_in);
        module._free(ptr_out);
        module._free(grad_ptr);
    }

    function redraw() {
        const file = file_input.files[0];
        if (!file) return;
        const img = new Image();
        img.onload = () => updateImageAndRedraw(img);
        img.src = URL.createObjectURL(file);
    }

    file_input.addEventListener("change", redraw);

    document.querySelectorAll('input[name="c_mode"], input[name="mode"]').forEach(el => {
        el.addEventListener("change", redraw);
    });
    document.getElementById("grad_len").addEventListener("input", redraw);
});

const font_size_range = document.getElementById('font_size');
font_size_range.addEventListener("input", (e) => {
    const font_size = font_size_range.value;
    const out_text = document.getElementById("out_text");
    out_text.style.fontSize = font_size + "px";
});
};
