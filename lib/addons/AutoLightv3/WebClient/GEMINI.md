# Ringkasan Proyek: AutoLight V3 WebClient

## Tujuan Proyek

**AutoLight V3 WebClient** adalah antarmuka web modern yang dibuat dengan Next.js 15 dan React 19 untuk mengontrol dan memantau sistem pencahayaan perangkat keras "AutoLight V3". Aplikasi ini memungkinkan pengguna untuk secara *real-time* mengubah mode, menyesuaikan pengaturan, dan melihat status perangkat dari browser atau aplikasi seluler.

Proyek ini merupakan hasil migrasi dari versi sebelumnya yang menggunakan Vue 3, dan sekarang sepenuhnya berjalan di atas tumpukan teknologi React modern dengan performa yang dioptimalkan.

## Arsitektur & Teknologi

- **Framework**: Next.js 15 (dengan App Router)
- **Library UI**: React 19
- **Bahasa**: TypeScript (Strict Mode)
- **Styling**: Tailwind CSS dengan `shadcn/ui` untuk komponen UI yang aksesibel dan dapat disesuaikan.
- **Manajemen State**: React Context (`ALSContext`) dikombinasikan dengan Custom Hooks (`useALSPolling`) untuk manajemen state global dan pengambilan data.
- **Build Tool**: Turbopack untuk build yang cepat.
- **Deployment**: Didesain untuk ekspor statis (`output: 'export'`) dan dapat di-deploy sebagai situs web mandiri atau dibungkus dalam aplikasi seluler menggunakan **Capacitor**.

## Struktur Direktori Utama

```
/
├── app/                  # Halaman dan rute aplikasi (Next.js App Router)
│   ├── page.tsx          # Halaman utama
│   ├── layout.tsx        # Layout utama aplikasi
│   └── settings/         # Rute untuk halaman pengaturan
├── components/           # Komponen React yang dapat digunakan kembali
│   ├── MainPage.tsx      # Komponen inti yang mengatur logika utama
│   ├── ModernTheme.tsx   # Tampilan UI modern berbasis kartu
│   ├── ClassicTheme.tsx  # Tampilan UI klasik
│   └── ui/               # Komponen dari shadcn/ui
├── lib/                  # Logika inti, konteks, dan hook
│   ├── contexts/
│   │   └── ALSContext.tsx  # State global untuk data perangkat dan pengaturan UI
│   ├── hooks/
│   │   └── useALSPolling.ts # Hook untuk polling data dari perangkat secara periodik
│   └── api/
│       └── device.ts     # Fungsi untuk berinteraksi dengan API perangkat
├── public/               # Aset statis
└── android/              # Proyek Android yang dihasilkan oleh Capacitor
```

## Fungsionalitas Utama

1.  **Kontrol Perangkat Keras**:
    - Mengubah mode operasi perangkat (16 mode, termasuk ON/OFF).
    - Menyesuaikan *delay* atau waktu tunda perangkat melalui slider.
    - Mengubah nama perangkat yang ditampilkan.

2.  **Pemantauan Real-time**:
    - Menggunakan mekanisme *polling* setiap 2 detik untuk mengambil data terbaru dari perangkat keras melalui API HTTP.
    - Menampilkan status perangkat saat ini, seperti mode, *delay*, nama, dan nomor seri.

3.  **Antarmuka Pengguna Dinamis**:
    - **Dua Pilihan Tema**: "Modern" (berbasis kartu) dan "Klasik".
    - **Mode Gelap/Terang**: Dukungan penuh untuk beralih antara tema gelap dan terang.
    - **Desain Responsif**: Tampilan yang beradaptasi untuk desktop dan perangkat seluler.

4.  **Integrasi Seluler**:
    - Dikonfigurasi dengan **Capacitor** untuk membangun aplikasi Android asli dari basis kode web.
    - Termasuk konfigurasi keamanan jaringan untuk memungkinkan komunikasi dengan perangkat keras di jaringan lokal (`192.168.4.1`).

## Cara Menjalankan Proyek

1.  **Instalasi Dependensi**:
    ```bash
    npm install
    ```

2.  **Menjalankan Server Pengembangan**:
    Aplikasi akan berjalan di `http://localhost:3001`.
    ```bash
    npm run dev
    ```

3.  **Membangun untuk Produksi**:
    ```bash
    npm run build
    ```

4.  **Membangun Aplikasi Android (dengan Capacitor)**:
    Perintah ini akan membangun aplikasi web, menyinkronkannya dengan proyek Android, dan membukanya di Android Studio.
    ```bash
    npm run cap:build:android
    ```

## Komunikasi dengan Perangkat Keras

- Aplikasi berkomunikasi dengan perangkat AutoLight V3 melalui **REST API** pada alamat IP yang di-hardcode (namun dapat dikonfigurasi) `http://192.168.4.1:8000`.
- Hook `useALSPolling` bertanggung jawab untuk mengambil data dari endpoint seperti `/api/v1/data/get/mode` dan `/api/v1/data/get/delay` setiap 2 detik untuk menjaga data tetap sinkron.
