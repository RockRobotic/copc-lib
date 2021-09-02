#include <copc-lib/las/point.hpp>

#include <sstream>

namespace copc::las {

Point::Point(const uint8_t &point_format_id) : point_format_id_(point_format_id) {
    point_10_ = nullptr;
    point_14_ = nullptr;
    rgb_ = nullptr;
    gps_time_ = nullptr;
    nir_ = nullptr;

    switch (point_format_id) {
        case 0:point_10_ = new internal::Point10();
            break;
        case 1:point_10_ = new internal::Point10();
            gps_time_ = new internal::GpsTime();
            break;
        case 2:point_10_ = new internal::Point10();
            rgb_ = new internal::Rgb();
            break;
        case 3:point_10_ = new internal::Point10();
            rgb_ = new internal::Rgb();
            gps_time_ = new internal::GpsTime();
            break;
        case 4:point_10_ = new internal::Point10();
            gps_time_ = new internal::GpsTime();
            break;
        case 5:point_10_ = new internal::Point10();
            rgb_ = new internal::Rgb();
            gps_time_ = new internal::GpsTime();
            break;
        case 6:point_14_ = new internal::Point14();
            gps_time_ = new internal::GpsTime();
            break;
        case 7:point_14_ = new internal::Point14();
            rgb_ = new internal::Rgb();
            gps_time_ = new internal::GpsTime();
            break;
        case 8:point_14_ = new internal::Point14();
            rgb_ = new internal::Rgb();
            gps_time_ = new internal::GpsTime();
            nir_ = new internal::Nir();
            break;
        case 9:point_14_ = new internal::Point14();
            gps_time_ = new internal::GpsTime();
            break;
        case 10:point_14_ = new internal::Point14();
            rgb_ = new internal::Rgb();
            gps_time_ = new internal::GpsTime();
            nir_ = new internal::Nir();
            break;
        default:throw std::runtime_error("Point format must be 0-10");
    }

    point_byte_size_ = BaseByteSize(point_format_id);
}

// Copy constructor
Point::Point(const Point &point) : point_format_id_(point.point_format_id_), point_byte_size_(point.point_byte_size_) {

    point_10_ = nullptr;
    point_14_ = nullptr;
    gps_time_ = nullptr;
    rgb_ = nullptr;
    nir_ = nullptr;

    if (point.HasPoint10())
        point_10_ = new internal::Point10(point.GetPoint10());
    else
        point_14_ = new internal::Point14(point.GetPoint14());
    if (point.HasGpsTime())
        gps_time_ = new internal::GpsTime();
    if (point.HasRgb())
        rgb_ = new internal::Rgb(point.GetRgb());
    if (point.HasNir())
        nir_ = new internal::Nir(point.GetNir());
}

void Point::Unpack(std::istream &in_stream, const uint16_t &point_record_length) {
    if (point_10_)
        point_10_->Unpack(in_stream);
    else
        point_14_->Unpack(in_stream);
    if (gps_time_)
        gps_time_->Unpack(in_stream);
    if (rgb_)
        rgb_->Unpack(in_stream);
    if (nir_)
        nir_->Unpack(in_stream);

    for (uint16_t i = 0; i < (point_record_length - point_byte_size_); i++) {
        extra_bytes_.emplace_back(in_stream);
    }
}

void Point::Pack(std::ostream &out_stream) const {
    if (point_10_)
        point_10_->Pack(out_stream);
    else
        point_14_->Pack(out_stream);
    if (gps_time_)
        gps_time_->Pack(out_stream);
    if (rgb_)
        rgb_->Pack(out_stream);
    if (nir_)
        nir_->Pack(out_stream);
    for (auto eb: extra_bytes_)
        eb.Pack(out_stream);
}

uint8_t Point::BaseByteSize(const uint8_t &point_format_id) {
    switch (point_format_id) {
        case 0:return 20;
        case 1:return 28;
        case 2:return 26;
        case 3:return 34;
        case 4:return 28;
        case 5:return 34;
        case 6:return 30;
        case 7:return 36;
        case 8:return 38;
        case 9:return 30;
        case 10:return 38;
        default:return 0;
    }
}

std::string Point::ToString() const {
    std::stringstream ss;
    if (point_10_)
        ss << point_10_->ToString();
    else
        ss << point_14_->ToString();
    if (gps_time_)
        ss << std::endl << gps_time_->ToString();
    if (rgb_)
        ss << std::endl << rgb_->ToString();
    if (nir_)
        ss << std::endl << nir_->ToString();
    if (!extra_bytes_.empty())
        ss << std::endl << "Extra Bytes: " << extra_bytes_.size();

    return ss.str();
}

void Point::ToPointFormat(const uint8_t &point_format_id) {

    switch (point_format_id) {
        case 0:
            if (point_format_id_ > 5) {
                point_10_ = new internal::Point10(point_14_->ToPoint10());
                delete point_14_;
                point_14_ = nullptr;
            }
            if (gps_time_ != nullptr) {
                delete gps_time_;
                gps_time_ = nullptr;
            }
            if (rgb_) {
                delete rgb_;
                rgb_ = nullptr;
            }
            if (nir_) {
                delete nir_;
                nir_ = nullptr;
            }
            break;
        case 1:
            if (point_format_id_ > 5) {
                point_10_ = new internal::Point10(point_14_->ToPoint10());
                delete point_14_;
                point_14_ = nullptr;
            }
            if (!gps_time_)
                gps_time_ = new internal::GpsTime;
            if (rgb_) {
                delete rgb_;
                rgb_ = nullptr;
            }
            if (nir_) {
                delete nir_;
                nir_ = nullptr;
            }
            break;
        case 2:
            if (point_format_id_ > 5) {
                point_10_ = new internal::Point10(point_14_->ToPoint10());
                delete point_14_;
                point_14_ = nullptr;
            }
            if (gps_time_) {
                delete gps_time_;
                gps_time_ = nullptr;
            }
            if (!rgb_)
                rgb_ = new internal::Rgb();
            if (nir_) {
                delete nir_;
                nir_ = nullptr;
            }
            break;
        case 3:
            if (point_format_id_ > 5) {
                point_10_ = new internal::Point10(point_14_->ToPoint10());
                delete point_14_;
                point_14_ = nullptr;
            }
            if (!gps_time_)
                gps_time_ = new internal::GpsTime;
            if (!rgb_)
                rgb_ = new internal::Rgb();
            if (nir_) {
                delete nir_;
                nir_ = nullptr;
            }
            break;
        case 4:
            if (point_format_id_ > 5) {
                point_10_ = new internal::Point10(point_14_->ToPoint10());
                delete point_14_;
                point_14_ = nullptr;
            }
            if (!gps_time_)
                gps_time_ = new internal::GpsTime;
            if (rgb_) {
                delete rgb_;
                rgb_ = nullptr;
            }
            if (nir_) {
                delete nir_;
                nir_ = nullptr;
            }
            break;
        case 5:
            if (point_format_id_ > 5) {
                point_10_ = new internal::Point10(point_14_->ToPoint10());
                delete point_14_;
                point_14_ = nullptr;

            }
            if (!gps_time_)
                gps_time_ = new internal::GpsTime;
            if (!rgb_)
                rgb_ = new internal::Rgb();
            if (nir_) {
                delete nir_;
                nir_ = nullptr;
            }
            break;
        case 6:
            if (point_format_id_ < 6) {
                point_14_ = new internal::Point14(point_10_->ToPoint14());
                delete point_10_;
                point_10_ = nullptr;
            }
            if (!gps_time_)
                gps_time_ = new internal::GpsTime;
            if (rgb_) {
                delete rgb_;
                rgb_ = nullptr;
            }
            if (nir_) {
                delete nir_;
                nir_ = nullptr;
            }
            break;
        case 7:
            if (point_format_id_ < 6) {
                point_14_ = new internal::Point14(point_10_->ToPoint14());
                delete point_10_;
                point_10_ = nullptr;
            }
            if (!gps_time_)
                gps_time_ = new internal::GpsTime;
            if (!rgb_)
                rgb_ = new internal::Rgb();
            if (nir_) {
                delete nir_;
                nir_ = nullptr;
            }
            break;
        case 8:
            if (point_format_id_ < 6) {
                point_14_ = new internal::Point14(point_10_->ToPoint14());
                delete point_10_;
                point_10_ = nullptr;
            }
            if (!gps_time_)
                gps_time_ = new internal::GpsTime;
            if (!rgb_)
                rgb_ = new internal::Rgb();
            if (!nir_)
                nir_ = new internal::Nir();
            break;
        case 9:
            if (point_format_id_ < 6) {
                point_14_ = new internal::Point14(point_10_->ToPoint14());
                delete point_10_;
                point_10_ = nullptr;
            }
            if (!gps_time_)
                gps_time_ = new internal::GpsTime;
            if (rgb_) {
                delete rgb_;
                rgb_ = nullptr;
            }
            if (nir_) {
                delete nir_;
                nir_ = nullptr;
            }
            break;
        case 10:
            if (point_format_id_ < 6) {
                point_14_ = new internal::Point14(point_10_->ToPoint14());
                delete point_10_;
                point_10_ = nullptr;
            }
            if (!gps_time_)
                gps_time_ = new internal::GpsTime;
            if (!rgb_)
                rgb_ = new internal::Rgb();
            if (!nir_)
                nir_ = new internal::Nir();
            break;
        default:throw std::runtime_error("Point format must be 0-10");
    }

    point_format_id_ = point_format_id;
    point_byte_size_ = BaseByteSize(point_format_id);
}

} // namespace copc::las