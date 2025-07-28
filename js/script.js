window.onload = () => {
Module().then((module) => {
    const input = document.getElementById("file_input");
    const canvas = document.getElementById("canvas");
    const ctx = canvas.getContext("2d");

    console.log(input);

    input.addEventListener("change", (e) => {
        const file = e.target.files[0];
        const img = new Image();

        img.onload = () => {
            const width = img.width;
            const height = img.height;

            canvas.width = width;
            canvas.height = height;
            ctx.drawImage(img, 0, 0);

            const { data } = ctx.getImageData(0, 0, width, height);
            const byte_length = data.length;

            const ptr_in = module._malloc(byte_length);
            module.HEAPU8.set(data, ptr_in);

            const { gradient } = document.getElementById("grad").textContent;

            const encoder = new TextEncoder();
            const gradient_bytes = encoder.encode(gradient);
            
            const grad_ptr = module._malloc(gradient_bytes.length)
            module.HEAPU8.set(gradient_bytes, grad_ptr);

            const grad_len = parseInt(document.getElementById("grad_len").textContent);
            
            const c_mode = document.querySelector('input[name="c_mode"]:checked').value;
            const mode = document.querySelector('input[name="mode"]:checked').value.charCodeAt(0);


            const ptr_out = module._imgToAscii(ptr_in, width, height, grad_ptr, grad_len, c_mode, mode);

            if(!ptr_out){
                console.log('Null pointer returned!');
                return;
            }
            
            const _width = width * (1 + (mode == '3' ? 1 : 0));
            const _height = height / (1 + (mode == '2' ? 1 : 0));
            const size = _width * _height + _height;
            const output = new Uint8Array(module.HEAPU8.buffer, ptr_out, size);
            const text = new TextDecoder("utf-8").decode(output);

            /*
            const result = [];
            for (let r = 0; r < height; r++) {
                const row = [];
                for (let c = 0; c < width; c++) {
                    const i = r * width + c;
                    const charCode = module.HEAPU8[ptr_out + i];
                    row.push(String.fromCharCode(charCode));
                }

                result.push(row);
            }
            
            const text = result.map(row => row.join("")).join("\n");
            */
            document.getElementById("out_text").innerText = text;

            module._free(ptr_in);
            module._free(ptr_out);
            module._free(grad_ptr);
        }
        img.src = URL.createObjectURL(file);
    });
});
};